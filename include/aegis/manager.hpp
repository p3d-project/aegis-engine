#pragma once

/**
 * @file manager.hpp
 * @brief Aegis Engine — `Manager`, the abstract base for singleton
 *        hardware-abstraction / heavy-computation / memory-management
 *        subsystems (the DOD side of the engine).
 *
 * @note Managers are the only code permitted to call platform/hardware
 *       functions directly — see `Component::SubmitToManager` for how
 *       Components hand data to them (one-way, not Pub/Sub).
 */

namespace aegis
{
// =============================================================================
// Manager
// =============================================================================

/**
 * @brief Abstract base for a singleton handling hardware abstraction, heavy
 *        computation, or memory management (the DOD side of the engine).
 *
 * Managers are the ONLY code allowed to call hardware-specific (platform)
 * functions.
 *
 * @note Manager intentionally does NOT derive from `etl::message_router`.
 *       Components submit to Managers via direct one-way calls, not
 *       Pub/Sub (see `Component::SubmitToManager`). Concrete Managers
 *       (e.g. `RenderManager`) each declare their own
 *       `SubmitData(const Payload::X&)` overload(s); those are not forced
 *       into this base class so adding a new payload type never requires
 *       touching `Manager` itself. Combine with `Singleton<TDerived>` for
 *       singleton behavior.
 */
class Manager
{
  public:
    virtual ~Manager() = default;

    /// Called once at startup.
    virtual void Init() = 0;

    /// Called once per frame, per the Core Engine Loop's "Process Managers" step.
    virtual void Process() = 0;

    /// Called once at shutdown.
    virtual void Shutdown() = 0;
};
} // namespace aegis
