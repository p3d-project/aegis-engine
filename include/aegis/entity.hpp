#pragma once

/**
 * @file entity.hpp
 * @brief Aegis Engine — `Entity`: an identity plus a fixed-capacity,
 *        non-owning list of attached Components, with typed
 *        (`getComponent<T>`) and untyped (`getComponentByID`) lookup.
 *
 * @note Entities do not own Component memory — see `Engine`'s pools.
 * @note Requires C++17.
 */

#include <aegis/component.hpp>
#include <aegis/types.hpp>
#include <cassert>
#include <etl/vector.h>

namespace aegis
{
// =============================================================================
// Entity
// =============================================================================

/**
 * @brief An identity plus a fixed-capacity, non-owning list of Components.
 *
 * @note Ownership: Components are allocated from a shared pool owned by `Engine`.
 *       Entity only stores pointers into that pool; it does not own the memory.
 */
class Entity
{
  public:
    Entity() = default;

    /// @param id This entity's unique identifier.
    explicit Entity(EntityID id) : entityID(id)
    {
    }

    /// @return This entity's unique identifier.
    EntityID getID() const
    {
        return entityID;
    }

    /**
     * @brief Registers an already-constructed Component with this entity
     *        and calls its `init()`.
     * @param c Pointer to a Component allocated by `Engine`.
     */
    void addComponent(Component* c);

    /**
     * @param type The `ComponentTypeID` to search for.
     * @return The first attached Component of the given type, or `nullptr`.
     */
    Component* getComponentByID(ComponentTypeID type) const;

    /**
     * @brief Retrieves and safely casts a Component of the specified type.
     * @tparam T The concrete Component type (must define `TYPE_ID`).
     * @return Pointer to the component, or nullptr if not found.
     */
    template <typename T> T* getComponent() const;

    /**
     * @brief Detaches the Component of the given type, calling its
     *        `destroy()` first.
     * @note Does NOT free pool memory (see `Engine::destroyComponent`),
     *       which is the recommended way to fully retire a component (it
     *       detaches from the owner, destroys, and frees the pool slot in
     *       one call without double-invoking `destroy()`).
     * @param type The `ComponentTypeID` to remove.
     */
    void removeComponentByID(ComponentTypeID type);

    /**
     * @brief Detaches and destroys a Component of the specified type.
     * @tparam T The concrete Component type (must define `TYPE_ID`).
     */
    template <typename T> void removeComponent();

    /**
     * @brief Removes a component pointer from this entity's internal list
     *        WITHOUT calling `destroy()` on it.
     *
     * Used by `Engine::destroyComponent` to keep an entity's component list
     * in sync when a component's lifecycle (Destroy + pool free) is being
     * managed externally. Prefer `removeComponent`/`Engine::destroyComponent`
     * for normal use; this is a low-level building block for those.
     *
     * This also clears the component's `owner` pointer so the detached
     * Component no longer reports a stale parent Entity after the removal.
     * @param c The component pointer to detach. No-op if not found.
     */
    void detachComponent(Component* c);

    /**
     * @brief Returns the number of Components currently attached to this
     *        Entity.
     * @return Number of live attached Components.
     */
    std::size_t getComponentCount() const
    {
        return components.size();
    }

    /**
     * @brief Returns the Component stored at a given slot index.
     * @param index Zero-based index into the attached-Component list.
     * @return Pointer to the requested Component, or `nullptr` if the index
     *         is out of range.
     */
    Component* getComponentAt(std::size_t index) const
    {
        return index < components.size() ? components[index] : nullptr;
    }

    /**
     * @brief Calls `update(dt)` on every attached Component, per the Core
     *        Engine Loop's "Update Components" step.
     * @param dt Fixed-point delta time for this frame.
     */
    void update(q20_12_t dt);

  private:
    EntityID entityID = 0;
    etl::vector<Component*, EngineLimits::MAX_COMPONENTS_PER_ENTITY> components;
};

// =============================================================================
// Entity method implementations
// =============================================================================

inline void Entity::addComponent(Component* c)
{
    if (c == nullptr || components.full())
    {
        return;
    }

    c->setOwner(this);
    c->init();
    components.push_back(c);
}

inline Component* Entity::getComponentByID(ComponentTypeID type) const
{
    for (Component* c : components)
    {
        if (c->getType() == type)
        {
            return c;
        }
    }
    return nullptr;
}

template <typename T> inline T* Entity::getComponent() const
{
    static_assert(detail::has_type_id<T>::value,
                  "\n\n[AE ERROR]: Component is missing its TYPE_ID\n"
                  "You must define: static constexpr ComponentTypeID TYPE_ID = ...\n");
    return static_cast<T*>(getComponentByID(T::TYPE_ID));
}

inline void Entity::removeComponentByID(ComponentTypeID type)
{
    for (auto it = components.begin(); it != components.end(); ++it)
    {
        if ((*it)->getType() == type)
        {
            Component* component = *it;
            component->destroy();
            component->setOwner(nullptr);
            components.erase(it);
            return;
        }
    }
}

template <typename T> inline void Entity::removeComponent()
{
    static_assert(detail::has_type_id<T>::value,
                  "\n\n[AE ERROR]: Component is missing its TYPE_ID\n"
                  "You must define: static constexpr ComponentTypeID TYPE_ID = ...\n");
    removeComponentByID(T::TYPE_ID);
}

inline void Entity::detachComponent(Component* c)
{
    for (auto it = components.begin(); it != components.end(); ++it)
    {
        if (*it == c)
        {
            c->setOwner(nullptr);
            components.erase(it);
            return;
        }
    }
}

inline void Entity::update(q20_12_t dt)
{
    for (Component* c : components)
    {
        if (c->IsActive())
        {
            c->update(dt);
        }
    }
}
} // namespace aegis
