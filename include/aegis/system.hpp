#pragma once

/**
 * @file system.hpp
 * @brief Aegis Engine — `System`, the abstract base for singleton game-rule
 *        handlers (Battle, Dialogue, UI logic, ...), and `SystemRouter
 *        TDerived, TMessageTypes...>`, the CRTP mixin that opts a concrete
 *        System into Pub/Sub.
 *
 * @note Mirrors `component.hpp`'s `ComponentRouter` design — see its
 *       documentation for the shared CRTP contract.
 */
#include <aegis/types.hpp>
#include <etl/message_router.h>

namespace aegis
{
// =============================================================================
// System
// =============================================================================

/**
 * @brief Abstract base for a singleton handling one slice of game rules /
 *        state (e.g. Battle, Dialogue, UILogic).
 *
 * Talks to Components via two-way Pub/Sub only. Like `Component`, this base
 * knows nothing about specific message types (see `SystemRouter`).
 */
class System
{
  public:
    virtual ~System() = default;

    /// Called once at startup.
    virtual void Init() = 0;

    /**
     * @brief Per-frame update, called from `Engine::Tick`.
     * @param dt Fixed-point delta time for this frame.
     */
    virtual void Update(fixed_t dt) = 0;

    /// Called once at shutdown.
    virtual void Shutdown() = 0;

    /**
     * @brief Returns this system as an `etl::imessage_router` if it
     *        participates in Pub/Sub, or `nullptr` if it doesn't.
     */
    virtual etl::imessage_router* AsMessageRouter()
    {
        return nullptr;
    }

  protected:
    System() = default;

    /**
     * @brief Whether this system's `Update` should run its logic this
     *        frame. Toggled by concrete subclasses in response to events
     *        received from `engineBus` (e.g. a
     *        "Start[SystemName]System" event).
     */
    bool isActive = false;
};

/**
 * @brief CRTP mixin that gives a concrete System Pub/Sub capability.
 *
 * Mirrors `ComponentRouter`; see its documentation for the CRTP contract.
 * A concrete System is typically also combined with `Singleton<TDerived>`:
 * @code
 * class BattleSystem : public SystemRouter<BattleSystem, Event::Damage>,
 *                       public Singleton<BattleSystem>
 * {
 * public:
 *     BattleSystem() : SystemRouter(kBattleSystemRouterID) {}
 *     void on_receive(const Event::Damage& msg) { ... }
 *     void on_receive_unknown(const etl::imessage&) {}
 *     // ... Init/Update/Shutdown ...
 * };
 * @endcode
 *
 * @tparam TDerived      The concrete, final System subclass.
 * @tparam TMessageTypes The Event types this system subscribes to/emits.
 */
template <typename TDerived, typename... TMessageTypes>
class SystemRouter : public System, public etl::message_router<TDerived, TMessageTypes...>
{
  public:
    /// @param routerID A unique id for this System (systems are singletons).
    explicit SystemRouter(etl::message_router_id_t routerID) : etl::message_router<TDerived, TMessageTypes...>(routerID)
    {
    }

    etl::imessage_router* AsMessageRouter() override
    {
        return static_cast<etl::message_router<TDerived, TMessageTypes...>*>(this);
    }
};
} // namespace aegis
