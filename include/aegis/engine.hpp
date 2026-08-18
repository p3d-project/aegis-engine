#pragma once

/**
 * @file engine.hpp
 * @brief Aegis Engine — `Engine<...>`: owns the Entity/Component pools,
 *        the System/Manager registry, and drives the Core Engine Loop
 *        (poll input -> update systems -> update components -> process
 *        managers -> compute).
 *
 * @note All pool/registry capacities are template parameters, not
 *       hardcoded constants — size `Engine` per project/platform via
 *       `using GameEngine = Engine<sizeof(Largest), alignof(Largest)>;`.
 * @note Depends on complete definitions of `Entity`, `Component`, `System`,
 *       and `Manager` (not just the forward declarations in `types.hpp`) —
 *       include those headers, or the aggregate `aegis.hpp`, before this one.
 */

#include <aegis/component.hpp>
#include <aegis/entity.hpp>
#include <aegis/manager.hpp>
#include <aegis/system.hpp>
#include <aegis/types.hpp>
#include <etl/generic_pool.h>
#include <etl/pool.h>

namespace aegis
{
// =============================================================================
// Engine
// =============================================================================

/**
 * @brief Owns global engine memory (Entity/Component pools), the registry
 *        of Systems/Managers, and drives the Core Engine Loop.
 *
 * `Engine` deliberately knows nothing about specific Systems, Managers,
 * Components, or message types. Those are all registered/created by game code at
 * runtime. This keeps the engine itself fully hardware and game-agnostic.
 *
 * All capacities are template parameters (not hardcoded constants) so each
 * project/platform can size its pools appropriately, e.g.:
 * @code
 * using GameEngine = Engine<sizeof(LargestComponent), alignof(LargestComponent)>;
 * GameEngine engine;
 * @endcode
 *
 * @tparam ComponentBlockSize  Byte size of the largest concrete Component
 *                           type the game will allocate (`sizeof(T)`).
 * @tparam ComponentBlockAlign Alignment of the largest concrete Component
 *                           type the game will allocate (`alignof(T)`).
 * @tparam MaxEntities         Max simultaneous Entities.
 * @tparam MaxComponents       Max simultaneous Components (all entities combined).
 * @tparam MaxSystems          Max registered Systems.
 * @tparam MaxManagers         Max registered Managers.
 */
template <std::size_t ComponentBlockSize,
          std::size_t ComponentBlockAlign,
          std::size_t MaxEntities = EngineLimits::MAX_ENTITIES,
          std::size_t MaxComponents = EngineLimits::MAX_COMPONENTS,
          std::size_t MaxSystems = EngineLimits::MAX_SYSTEMS,
          std::size_t MaxManagers = EngineLimits::MAX_MANAGERS>
class Engine
{
  public:
    Engine() = default;

    // --- Entity lifecycle -----------------------------------------------

    /// @return A freshly allocated, empty Entity, or `nullptr` if the entity pool/registry is full.
    Entity* createEntity()
    {
        if (activeEntities.full())
        {
            return nullptr;
        }

        Entity* entity = entityPool.create(nextEntityID++);
        if (entity != nullptr)
        {
            activeEntities.push_back(entity);
        }
        return entity;
    }

    /**
     * @brief Destroys all of the entity's components and returns the
     *        entity itself to the pool.
     * @param entity Entity previously returned by `createEntity`.
     */
    void destroyEntity(Entity* entity)
    {
        if (entity == nullptr)
        {
            return;
        }

        entity->destroy();

        for (auto it = activeEntities.begin(); it != activeEntities.end(); ++it)
        {
            if (*it == entity)
            {
                activeEntities.erase(it);
                break;
            }
        }

        entityPool.destroy(entity);
    }

    // --- Component lifecycle ---------------------------------------------

    /**
     * @brief Allocates a Component of concrete type `T` from the shared
     *        component pool, forwarding constructor arguments.
     * @tparam T    Concrete Component subclass to construct.
     * @tparam Args Constructor argument types, forwarded to `T`'s constructor.
     * @return Pointer to the new component, or `nullptr` if the pool is exhausted.
     */
    template <typename T, typename... Args> T* createComponent(Args&&... args)
    {
        static_assert(std::is_base_of<Component, T>::value, "\n\n[AE ERROR]: T must derive from Component");
        static_assert(detail::has_type_id<T>::value,
                      "\n\n[AE ERROR]: Component is missing its TYPE_ID\n"
                      "You must define: static constexpr ComponentTypeID TYPE_ID = ...\n");

        T* component = componentPool.template create<T>(std::forward<Args>(args)...);
        if (component != nullptr)
        {
            registerComponentForMessaging(component);
        }
        return component;
    }

    /**
     * @brief Fully retires a component: calls `destroy()`, detaches it from
     *        its owning Entity (if any), unsubscribes it from `engineBus`
     *        (if applicable), and returns its memory to the pool.
     *
     * This is the recommended way to destroy a component. It keeps the
     * owning Entity's internal list in sync, unlike freeing the pool slot
     * directly.
     * @tparam T Concrete Component subclass (must match the allocated type).
     * @param component Pointer previously returned by `createComponent`.
     */
    template <typename T> void destroyComponent(T* component)
    {
        if (component == nullptr)
        {
            return;
        }

        component->destroy();

        if (Entity* owner = component->getOwner())
        {
            owner->detachComponent(component);
        }

        if (etl::imessage_router* router = component->asMessageRouter())
        {
            engineBus.unsubscribe(*router);
        }

        componentPool.destroy(component);
    }

    /**
     * @brief Subscribes a component to `engineBus` if it participates in
     *        Pub/Sub. Called automatically by `createComponent`; exposed
     *        publicly for components constructed outside the pool (e.g. in
     *        unit tests).
     * @param component The component to register.
     */
    void registerComponentForMessaging(Component* component)
    {
        if (component == nullptr)
        {
            return;
        }

        if (etl::imessage_router* router = component->asMessageRouter())
        {
            engineBus.subscribe(*router);
        }
    }

    // --- System / Manager registration -----------------------------------

    /**
     * @brief Registers a System so `tick` calls its `update` each frame,
     *        subscribing it to `engineBus` if it participates in Pub/Sub.
     * @param system Typically `&SomeSystem::getInstance()`.
     */
    void registerSystem(System* system)
    {
        if (system == nullptr || systems.full())
        {
            return;
        }

        systems.push_back(system);

        if (etl::imessage_router* router = system->asMessageRouter())
        {
            engineBus.subscribe(*router);
        }
    }

    /**
     * @brief Registers a Manager so `tick` calls its `process` each frame.
     * @param manager Typically `&SomeManager::getInstance()`.
     */
    void registerManager(Manager* manager)
    {
        if (manager == nullptr || managers.full())
        {
            return;
        }

        managers.push_back(manager);
    }

    // --- Loop control -------------------------------------------------------

    /// Calls `init()` on every registered System, then every registered Manager.
    void initAll()
    {
        for (System* s : systems)
        {
            s->init();
        }
        for (Manager* m : managers)
        {
            m->init();
        }
    }

    /// Calls `shutdown()` on every registered Manager, then every registered System.
    void shutdownAll()
    {
        for (Manager* m : managers)
        {
            m->shutdown();
        }
        for (System* s : systems)
        {
            s->shutdown();
        }
    }

    /**
     * @brief Toggles the input polling
     * @param enabled
     */
    void setPollingEnabled(bool enabled)
    {
        isPollingEnabled = enabled;
    }

    /**
     * @brief Toggles the compute execution
     * @param enabled
     */
    void setComputeEnabled(bool enabled)
    {
        isComputeEnabled = enabled;
    }

    /**
     * @brief Installs the platform-specific input-polling hook, called at
     *        the start of every `tick`. Left injectable (rather than
     *        hardcoded) to keep `Engine` hardware-agnostic. Only Managers
     *        may touch hardware directly, and the function supplied here is
     *        expected to itself delegate to a Manager.
     */
    void setPollInputCallback(void (*fn)())
    {
        pollInputFn = fn;
    }

    /**
     * @brief Installs the platform-specific "push final data to hardware"
     *        hook, called at the end of every `tick`. Same rationale as
     *        `setPollInputCallback`.
     */
    void setComputeCallback(void (*fn)())
    {
        computeFn = fn;
    }

    /**
     * @brief Runs one iteration of the Core Engine Loop:
     *        poll input -> update systems -> update components -> process managers -> compute.
     * @param dt Fixed-point delta time for this frame.
     */
    void tick(fixed_t dt)
    {
        if (isPollingEnabled)
        {
            /// Assert that pollInputFn is not undefined
            assert(pollInputFn != nullptr);
            pollInputFn();
        }

        for (System* s : systems)
        {
            if (s->isActive())
            {
                s->update(dt);
            }
        }

        for (Entity* e : activeEntities)
        {
            e->update(dt);
        }

        for (Manager* m : managers)
        {
            m->process();
        }

        if (isComputeEnabled)
        {
            /// Assert that computeFn is not undefined
            assert(computeFn != nullptr);
            computeFn();
        }
    }

  private:
    etl::pool<Entity, MaxEntities> entityPool;
    etl::generic_pool<ComponentBlockSize, ComponentBlockAlign, MaxComponents> componentPool;

    etl::vector<Entity*, MaxEntities> activeEntities;
    etl::vector<System*, MaxSystems> systems;
    etl::vector<Manager*, MaxManagers> managers;

    EntityID nextEntityID = 1;
    bool isPollingEnabled = true;
    bool isComputeEnabled = true;

    void (*pollInputFn)() = nullptr;
    void (*computeFn)() = nullptr;
};
} // namespace aegis
