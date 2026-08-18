#pragma once

/**
 * @file entity.hpp
 * @brief Aegis Engine — `Entity`: an identity plus a fixed-capacity,
 *        non-owning list of attached Components, with typed
 *        (`GetComponent<T>`) and untyped (`GetComponentByID`) lookup.
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
     *        and calls its `Init()`.
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
     *        `Destroy()` first.
     * @note Does NOT free pool memory (see `Engine::DestroyComponent`),
     *       which is the recommended way to fully retire a component (it
     *       detaches from the owner, destroys, and frees the pool slot in
     *       one call without double-invoking `Destroy()`).
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
     *        WITHOUT calling `Destroy()` on it.
     *
     * Used by `Engine::DestroyComponent` to keep an entity's component list
     * in sync when a component's lifecycle (Destroy + pool free) is being
     * managed externally. Prefer `RemoveComponent`/`Engine::DestroyComponent`
     * for normal use; this is a low-level building block for those.
     * @param c The component pointer to detach. No-op if not found.
     */
    void detachComponent(Component* c);

    /**
     * @brief Calls `Update(dt)` on every attached Component, per the Core
     *        Engine Loop's "Update Components" step.
     * @param dt Fixed-point delta time for this frame.
     */
    void update(fixed_t dt);

    /// Destroys (but does not free) all attached components.
    void destroy();

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
    return static_cast<T*>(GetComponentByID(T::TYPE_ID));
}

inline void Entity::removeComponentByID(ComponentTypeID type)
{
    for (auto it = components.begin(); it != components.end(); ++it)
    {
        if ((*it)->getType() == type)
        {
            (*it)->destroy();
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
            components.erase(it);
            return;
        }
    }
}

inline void Entity::update(fixed_t dt)
{
    for (Component* c : components)
    {
        if (c->isActive())
        {
            c->update(dt);
        }
    }
}

inline void Entity::destroy()
{
    for (Component* c : components)
    {
        c->destroy();
    }
    components.clear();
}
} // namespace aegis
