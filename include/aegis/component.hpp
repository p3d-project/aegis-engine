#pragma once

/**
 * @file component.hpp
 * @brief Aegis Engine — `Component`, the abstract base for gameplay
 *        "plugins" attached to Entities, and `ComponentRouter<TDerived,
 *        TMessageTypes...>`, the CRTP mixin that opts a concrete Component
 *        into Pub/Sub.
 *
 * @note `Component` itself has zero knowledge of message types; only
 *       concrete subclasses (via `ComponentRouter`) declare which events
 *       they subscribe to. See `types.hpp` for `BroadcastEvent`, the
 *       corresponding "send" half of Pub/Sub.
 */

#include "types.hpp"
#include <etl/message_router.h>

namespace aegis
{
// =============================================================================
// Component
// =============================================================================

/**
 * @brief Abstract base for all gameplay-facing "plugins".
 *
 * Bridges an Entity to Systems (via Pub/Sub) and to Managers (via a direct
 * one-way call). Structure: inherits from this abstract base only
 * (1-level max).
 *
 * @note This class intentionally knows nothing about specific message
 *       types. A Component that needs Pub/Sub should instead derive from
 *       `ComponentRouter<TDerived, TMessageTypes...>` (below), which mixes
 *       this interface together with an `etl::message_router` templated on
 *       whatever event types that concrete component cares about. A
 *       Component with no messaging needs can derive from `Component`
 *       directly.
 */
class Component
{
  public:
    virtual ~Component() = default;

    /// Called once when the component is attached to an Entity.
    virtual void Init() = 0;

    /**
     * @brief Per-frame update, called from `Entity::Update`.
     * @param dt Fixed-point delta time for this frame.
     */
    virtual void Update(fixed_t dt) = 0;

    /// Called when the component is detached/destroyed.
    virtual void Destroy() = 0;

    /// @return This component's game-defined type identifier.
    virtual ComponentTypeID GetType() const = 0;

    /// @return The Entity this component is attached to, or `nullptr`.
    Entity* GetOwner() const
    {
        return owner;
    }

    /// @param entity The Entity that now owns this component.
    void SetOwner(Entity* entity)
    {
        owner = entity;
    }

    /**
     * @brief Returns this component as an `etl::imessage_router` if it
     *        participates in Pub/Sub, or `nullptr` if it doesn't.
     *
     * `Engine` uses this to decide whether to subscribe/unsubscribe a
     * component to/from `engineBus`. Overridden by `ComponentRouter`.
     */
    virtual etl::imessage_router* AsMessageRouter()
    {
        return nullptr;
    }

  protected:
    /**
     * @brief One-way push of this component's data to its associated
     *        Manager(s).
     *
     * Deliberately takes no generic parameter: each concrete component
     * knows exactly which Manager singleton and payload type it submits to
     * (e.g. a MeshComponent calls
     * `RenderManager::GetInstance().SubmitData(Payload::RenderMesh{...})`).
     * A single virtual signature can't express "a different payload type
     * per component" without type erasure, which we want to avoid.
     */
    virtual void SubmitToManager() = 0;

    /// The Entity that owns this component (set via `SetOwner`).
    Entity* owner = nullptr;
};

/**
 * @brief CRTP mixin that gives a concrete Component Pub/Sub capability.
 *
 * Game code defines its own event types (see the companion example header)
 * and lists exactly the ones a given component cares about, e.g.:
 * @code
 * class HealthComponent : public ComponentRouter<HealthComponent, Event::Damage>
 * {
 * public:
 *     void on_receive(const Event::Damage& msg) { ... }
 *     void on_receive_unknown(const etl::imessage&) {}
 *     // ... Init/Update/Destroy/GetType/SubmitToManager ...
 * };
 * @endcode
 *
 * @note Per `etl::message_router`'s CRTP contract, `TDerived` (the final
 *       concrete class, NOT this mixin) must define `on_receive(...)` for
 *       every type in `TMessageTypes` plus `on_receive_unknown(...)`.
 *
 * @tparam TDerived      The concrete, final Component subclass.
 * @tparam TMessageTypes The Event types this component subscribes to/emits.
 */
template <typename TDerived, typename... TMessageTypes>
class ComponentRouter : public Component, public etl::message_router<TDerived, TMessageTypes...>
{
  public:
    /// @param routerID Defaults to an auto-assigned unique id per instance.
    explicit ComponentRouter(etl::message_router_id_t routerID = detail::NextComponentRouterID())
        : etl::message_router<TDerived, TMessageTypes...>(routerID)
    {
    }

    etl::imessage_router* AsMessageRouter() override
    {
        return static_cast<etl::message_router<TDerived, TMessageTypes...>*>(this);
    }
};
} // namespace aegis
