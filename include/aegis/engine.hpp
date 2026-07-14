#pragma once

/**
 * @file engine.hpp
 * @brief Aegis Engine — `Engine<...>`: owns the Entity/Component pools,
 *        the System/Manager registry, and drives the Core Engine Loop
 *        (Poll Input -> Update Systems -> Update Components -> Process
 *        Managers -> Compute).
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
 *                             type the game will allocate (`sizeof(T)`).
 * @tparam ComponentBlockAlign Alignment of the largest concrete Component
 *                             type the game will allocate (`alignof(T)`).
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
    Entity* CreateEntity()
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
     * @param entity Entity previously returned by `CreateEntity`.
     */
    void DestroyEntity(Entity* entity)
    {
        if (entity == nullptr)
        {
            return;
        }

        entity->Destroy();

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
    template <typename T, typename... Args> T* CreateComponent(Args&&... args)
    {
        static_assert(std::is_base_of<Component, T>::value, "\n\n[AE ERROR]: T must derive from Component");
        static_assert(detail::has_type_id<T>::value,
                      "\n\n[AE ERROR]: Component is missing its TYPE_ID\n"
                      "You must define: static constexpr ComponentTypeID TYPE_ID = ...\n");

        T* component = componentPool.template create<T>(std::forward<Args>(args)...);
        if (component != nullptr)
        {
            RegisterComponentForMessaging(component);
        }
        return component;
    }

    /**
     * @brief Fully retires a component: calls `Destroy()`, detaches it from
     *        its owning Entity (if any), unsubscribes it from `engineBus`
     *        (if applicable), and returns its memory to the pool.
     *
     * This is the recommended way to destroy a component. It keeps the
     * owning Entity's internal list in sync, unlike freeing the pool slot
     * directly.
     * @tparam T Concrete Component subclass (must match the allocated type).
     * @param component Pointer previously returned by `CreateComponent`.
     */
    template <typename T> void DestroyComponent(T* component)
    {
        if (component == nullptr)
        {
            return;
        }

        component->Destroy();

        if (Entity* owner = component->GetOwner())
        {
            owner->DetachComponent(component);
        }

        if (etl::imessage_router* router = component->AsMessageRouter())
        {
            engineBus.unsubscribe(*router);
        }

        componentPool.destroy(component);
    }

    /**
     * @brief Subscribes a component to `engineBus` if it participates in
     *        Pub/Sub. Called automatically by `CreateComponent`; exposed
     *        publicly for components constructed outside the pool (e.g. in
     *        unit tests).
     * @param component The component to register.
     */
    void RegisterComponentForMessaging(Component* component)
    {
        if (component == nullptr)
        {
            return;
        }

        if (etl::imessage_router* router = component->AsMessageRouter())
        {
            engineBus.subscribe(*router);
        }
    }

    // --- System / Manager registration -----------------------------------

    /**
     * @brief Registers a System so `Tick` calls its `Update` each frame,
     *        subscribing it to `engineBus` if it participates in Pub/Sub.
     * @param system Typically `&SomeSystem::GetInstance()`.
     */
    void RegisterSystem(System* system)
    {
        if (system == nullptr || systems.full())
        {
            return;
        }

        systems.push_back(system);

        if (etl::imessage_router* router = system->AsMessageRouter())
        {
            engineBus.subscribe(*router);
        }
    }

    /**
     * @brief Registers a Manager so `Tick` calls its `Process` each frame.
     * @param manager Typically `&SomeManager::GetInstance()`.
     */
    void RegisterManager(Manager* manager)
    {
        if (manager == nullptr || managers.full())
        {
            return;
        }

        managers.push_back(manager);
    }

    // --- Loop control -------------------------------------------------------

    /// Calls `Init()` on every registered System, then every registered Manager.
    void InitAll()
    {
        for (System* s : systems)
        {
            s->Init();
        }
        for (Manager* m : managers)
        {
            m->Init();
        }
    }

    /// Calls `Shutdown()` on every registered Manager, then every registered System.
    void ShutdownAll()
    {
        for (Manager* m : managers)
        {
            m->Shutdown();
        }
        for (System* s : systems)
        {
            s->Shutdown();
        }
    }

    /**
     * @brief Toggles the input polling
     * @param enabled
     */
    void SetPollingEnabled(bool enabled)
    {
        isPollingEnabled = enabled;
    }

    /**
     * @brief Toggles the compute execution
     * @param enabled
     */
    void SetComputeEnabled(bool enabled)
    {
        isComputeEnabled = enabled;
    }

    /**
     * @brief Installs the platform-specific input-polling hook, called at
     *        the start of every `Tick`. Left injectable (rather than
     *        hardcoded) to keep `Engine` hardware-agnostic. Only Managers
     *        may touch hardware directly, and the function supplied here is
     *        expected to itself delegate to a Manager.
     */
    void SetPollInputCallback(void (*fn)())
    {
        pollInputFn = fn;
    }

    /**
     * @brief Installs the platform-specific "push final data to hardware"
     *        hook, called at the end of every `Tick`. Same rationale as
     *        `SetPollInputCallback`.
     */
    void SetComputeCallback(void (*fn)())
    {
        computeFn = fn;
    }

    /**
     * @brief Runs one iteration of the Core Engine Loop:
     *        Poll Input -> Update Systems -> Update Components -> Process Managers -> Compute.
     * @param dt Fixed-point delta time for this frame.
     */
    void Tick(fixed_t dt)
    {
        if (isPollingEnabled)
        {
            /// Assert that pollInputFn is not undefined
            assert(pollInputFn != nullptr);
            pollInputFn();
        }

        for (System* s : systems)
        {
            s->Update(dt);
        }

        for (Entity* e : activeEntities)
        {
            e->Update(dt);
        }

        for (Manager* m : managers)
        {
            m->Process();
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
