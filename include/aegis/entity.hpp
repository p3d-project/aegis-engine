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
    EntityID GetID() const
    {
        return entityID;
    }

    /**
     * @brief Registers an already-constructed Component with this entity
     *        and calls its `Init()`.
     * @param c Pointer to a Component allocated by `Engine`.
     */
    void AddComponent(Component* c);

    /**
     * @param type The `ComponentTypeID` to search for.
     * @return The first attached Component of the given type, or `nullptr`.
     */
    Component* GetComponentByID(ComponentTypeID type) const;

    /**
     * @brief Retrieves and safely casts a Component of the specified type.
     * @tparam T The concrete Component type (must define `TYPE_ID`).
     * @return Pointer to the component, or nullptr if not found.
     */
    template <typename T> T* GetComponent() const;

    /**
     * @brief Detaches the Component of the given type, calling its
     *        `Destroy()` first.
     * @note Does NOT free pool memory (see `Engine::DestroyComponent`),
     *       which is the recommended way to fully retire a component (it
     *       detaches from the owner, destroys, and frees the pool slot in
     *       one call without double-invoking `Destroy()`).
     * @param type The `ComponentTypeID` to remove.
     */
    void RemoveComponentByID(ComponentTypeID type);

    /**
     * @brief Detaches and destroys a Component of the specified type.
     * @tparam T The concrete Component type (must define `TYPE_ID`).
     */
    template <typename T> void RemoveComponent();

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
    void DetachComponent(Component* c);

    /**
     * @brief Calls `Update(dt)` on every attached Component, per the Core
     *        Engine Loop's "Update Components" step.
     * @param dt Fixed-point delta time for this frame.
     */
    void Update(fixed_t dt);

    /// Destroys (but does not free) all attached components.
    void Destroy();

  private:
    EntityID entityID = 0;
    etl::vector<Component*, EngineLimits::MAX_COMPONENTS_PER_ENTITY> components;
};

// =============================================================================
// Entity method implementations
// =============================================================================

inline void Entity::AddComponent(Component* c)
{
    if (c == nullptr || components.full())
    {
        return;
    }

    c->SetOwner(this);
    c->Init();
    components.push_back(c);
}

inline Component* Entity::GetComponentByID(ComponentTypeID type) const
{
    for (Component* c : components)
    {
        if (c->GetType() == type)
        {
            return c;
        }
    }
    return nullptr;
}

template <typename T> inline T* Entity::GetComponent() const
{
    static_assert(detail::has_type_id<T>::value,
                  "\n\n[AE ERROR]: Component is missing its TYPE_ID\n"
                  "You must define: static constexpr ComponentTypeID TYPE_ID = ...\n");
    return static_cast<T*>(GetComponentByID(T::TYPE_ID));
}

inline void Entity::RemoveComponentByID(ComponentTypeID type)
{
    for (auto it = components.begin(); it != components.end(); ++it)
    {
        if ((*it)->GetType() == type)
        {
            (*it)->Destroy();
            components.erase(it);
            return;
        }
    }
}

template <typename T> inline void Entity::RemoveComponent()
{
    static_assert(detail::has_type_id<T>::value,
                  "\n\n[AE ERROR]: Component is missing its TYPE_ID\n"
                  "You must define: static constexpr ComponentTypeID TYPE_ID = ...\n");
    RemoveComponentByID(T::TYPE_ID);
}

inline void Entity::DetachComponent(Component* c)
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

inline void Entity::Update(fixed_t dt)
{
    for (Component* c : components)
    {
        c->Update(dt);
    }
}

inline void Entity::Destroy()
{
    for (Component* c : components)
    {
        c->Destroy();
    }
    components.clear();
}
} // namespace aegis
