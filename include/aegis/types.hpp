#pragma once

/**
 * @file types.hpp
 * @brief Aegis Engine — core type aliases, fixed-point math type, default
 *        engine capacity limits, the global Pub/Sub message bus, and the
 *        small CRTP/SFINAE helpers (`Singleton`, `has_type_id`) shared by
 *        every other Aegis header.
 *
 * @note This is the foundation header — every other file in the library
 *       depends on it, directly or transitively. It defines no game-specific
 *       data; game code supplies its own event, payload, and component-type
 *       definitions on top of these primitives.
 * @note Requires C++17 (inline variables, `std::void_t`).
 */

#include <cstdint>
#include <type_traits>

#include <etl/message_bus.h>
#include <fpm/fixed.hpp>

namespace aegis
{
// =============================================================================
// Core type aliases
// =============================================================================

/// Unique identifier for an Entity instance.
using EntityID = std::uint32_t;

/**
 * @brief Engine-wide fixed-point type (Q16.16 signed).
 *
 * Assume no FPU, so all game-logic math (positions, damage rolls, timers,
 * delta-time, etc.) must go through this type instead of float/double.
 */
using fixed_t = fpm::fixed<std::int32_t, std::int64_t, 16>;

/**
 * @brief Generic identifier for a concrete Component's "type".
 *
 * The engine only ever stores/compares this as an opaque integer; it has no
 * knowledge of what the values mean. Game code defines its own enum (e.g.
 * `enum class ComponentType : ComponentTypeID { Mesh, Hitbox, Health, ... }`)
 * and returns the underlying value from `Component::GetType()`.
 */
using ComponentTypeID = std::uint16_t;

// Forward declarations
class Entity;
class Component;
class System;
class Manager;

// =============================================================================
// Engine-wide default capacity limits
//
// Every container in this file is fixed-capacity (etl::vector / etl::pool)
// rather than std::vector/new/delete. These are only *defaults*: `Engine` is a
// template so a game can override any of them per-project without editing
// this header. See `Engine`'s template parameters below.
// =============================================================================
namespace EngineLimits
{
constexpr std::size_t MAX_COMPONENTS_PER_ENTITY = 8;
constexpr std::size_t MAX_ENTITIES = 256;
constexpr std::size_t MAX_COMPONENTS = 1024;
constexpr std::size_t MAX_SYSTEMS = 32;
constexpr std::size_t MAX_MANAGERS = 16;
constexpr std::size_t MAX_MESSAGE_ROUTERS = 64;
} // namespace EngineLimits

/**
 * @brief The single global Pub/Sub router for all Component <-> System
 *        communication
 *
 * Declared `inline` so this header can be included in multiple translation
 * units without needing a companion .cpp.
 */
inline etl::message_bus<EngineLimits::MAX_MESSAGE_ROUTERS> engineBus;

/**
 * @brief Publishes an event onto the global `engineBus` so every subscribed
 *        Component/System (i.e. every `ComponentRouter`/`SystemRouter`
 *        instance whose template argument list includes `TMessage`)
 *        receives it via its `on_receive(const TMessage&)`.
 *
 * This is the "send" half of the two-way Pub/Sub design - the counterpart
 * to the `on_receive(...)` overloads a component/system writes to
 * "receive". Call it from anywhere a Component or System wants to
 * broadcast, e.g.:
 * @code
 * BroadcastEvent(Event::Damage{15, 0});
 * @endcode
 *
 * @tparam TMessage An `etl::message<ID>`-derived event type (see the
 *                   `Event` namespace convention in the companion example).
 * @param msg The event instance to broadcast.
 */
template <typename TMessage> inline void BroadcastEvent(const TMessage& msg)
{
    engineBus.receive(msg);
}

namespace detail
{
/**
 * @brief Hands out unique router ids (valid range: 0-249, see ETL docs)
 *        for Component instances subscribing to `engineBus`.
 *
 * Systems/Managers are singletons and supply their own fixed id instead
 * (see `SystemRouter`), since a monotonic counter isn't needed for a
 * type that only ever has one instance.
 */
inline etl::message_router_id_t NextComponentRouterID()
{
    static etl::message_router_id_t next = 0;
    return next++;
}

/**
* @brief SFINAE trait to verify a Component has defined TYPE_ID.
*/
template <typename, typename = void> struct has_type_id : std::false_type
{
};

template <typename T> struct has_type_id<T, std::void_t<decltype(T::TYPE_ID)>> : std::true_type
{
};
} // namespace detail

// =============================================================================
// Singleton — CRTP helper for System/Manager singletons
// =============================================================================

/**
 * @brief CRTP mixin that gives a class a lazily-constructed, thread-safe
 *        singleton accessor.
 *
 * Each concrete System/Manager subclass (e.g. `BattleSystem`, `RenderManager`)
 * gets its own independent singleton instance by inheriting `Singleton<TDerived>`
 * with itself as `TDerived`. There is deliberately no single shared
 * "System instance" at the abstract-base level, since a game has many distinct
 * System/Manager subclasses.
 *
 * @tparam TDerived The concrete class inheriting this mixin.
 */
template <typename TDerived> class Singleton
{
  public:
    /**
     * @brief Returns the single instance of TDerived, constructing it on
     *        first use.
     */
    static TDerived& GetInstance()
    {
        static TDerived instance;
        return instance;
    }

  protected:
    Singleton() = default;
    ~Singleton() = default;

  public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};
} // namespace aegis

namespace ae = aegis;
