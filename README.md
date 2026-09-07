# Aegis Engine

Aegis Engine is a lightweight, strictly-bounded C++17 Entity-Component (EC) and Data-Oriented Design (DOD) hybrid game framework. It was originally engineered for the Persona 3 Dual (P3D) project, targeting the Nintendo DS platform.

Built for highly constrained embedded environments, Aegis guarantees zero dynamic memory allocation during the core game loop to prevent memory fragmentation, and strictly avoids floating-point math to accommodate hardware without a dedicated FPU.

The Aegis engine was named as such for two reasons:
- It is a play on the character *Aigis'* name in P3D
- It means *protection, backing, or sponsorship by a particularly powerful person, organization, or system* which plays into the idea that a game engine *backs* a game

> Want to help develop the engine? Join the [Discord!](https://discord.gg/CQnkc5gS6a) Any help, big or small, would be greatly appreciated!

![Stars](https://img.shields.io/github/stars/p3d-project/aegis-engine?style=flat-square&color=gold)
![Forks](https://img.shields.io/github/forks/p3d-project/aegis-engine?style=flat-square&color=blue)
![Last Commit](https://img.shields.io/github/last-commit/p3d-project/aegis-engine?style=flat-square&color=green)
![License](https://img.shields.io/github/license/p3d-project/aegis-engine?style=flat-square)

[![C++](https://img.shields.io/badge/C++-%2300599C.svg?logo=c%2B%2B&logoColor=white)](#)

[![Discord](https://img.shields.io/discord/1498850477545357482?label=Discord&logo=discord&style=flat-square&color=5865F2)](https://discord.gg/CQnkc5gS6a)

## Architecture

Aegis enforces a strict separation of concerns, heavily favoring composition over inheritance. The architecture is divided into the following pillars:

* **Engine (`ae::Engine`):** The core runtime. It owns all pre-allocated memory pools for Entities and Components, maintains the registries, and drives the deterministic execution loop.

* **Entity (`ae::Entity`):** A lightweight identifier that acts as an empty container (the host) for a fixed-capacity array of Component pointers.

* **Component (`ae::Component`):** Pluggable logic/data nodes that bridge Entities with Systems and Managers. Concrete components can opt into event messaging by inheriting from `ae::ComponentRouter`.

* **System (`ae::System`):** Singletons responsible for overarching game rules and state logic (e.g., `BattleSystem`, `DialogueSystem`). Systems communicate exclusively via a global two-way Pub/Sub event bus.

* **Manager (`ae::Manager`):** Singletons focused on hardware abstraction, heavy computation, and memory management (e.g., `RenderManager`, `AudioManager`). Managers do not use the event bus; instead, Components submit data directly to them. Managers are the *only* modules permitted to execute hardware-specific API calls, keeping the rest of the engine completely platform-agnostic.

### The Core Engine Loop

Execution follows a strict, unyielding sequence every frame:

1. **Poll Input:** Reads hardware states (buttons, touch).

2. **Update Systems:** Game logic processes the input and changes overarching states.

3. **Update Components:** Components translate system decisions into concrete data.

4. **Process Managers:** Data arrays are crunched (e.g., physics checks, animation interpolation).

5. **Compute:** Final data is pushed to the hardware layer (VRAM, audio buffers).


## Batteries Included (Dependencies)

Aegis bundles its dependencies under /libs, which are the following:

1. **[ETL (Embedded Template Library)](https://github.com/etlcpp/etl)**
* Aegis relies entirely on ETL for its pre-allocated memory pools (`etl::pool`, `etl::vector`) and its global Publisher/Subscriber communication routing (`etl::message_bus`).

2. **[FPM (Fixed Point Math)](https://github.com/MikeLankamp/fpm)**
* Handles all engine-wide math (positions, deltas, timers) using Q20.12 signed fixed-point integers (`ae::q20_12_t`), bypassing the performance penalty of software floating-point emulation on older ARM architecture.


## Quick Start
### 1. Define a Component and Event

```cpp
#include <aegis/component.hpp>
#include <aegis/types.hpp>

// Define a communication payload
namespace Event {
    struct Damage : public etl::message<1> {
        int amount;
    };
}

// Inherit from ComponentRouter to opt into Pub/Sub
class HealthComponent : public ae::ComponentRouter<HealthComponent, Event::Damage>
{
public:
    // Required compile-time ID
    static constexpr ae::ComponentTypeID TYPE_ID = 1;
    ae::ComponentTypeID GetType() const override { return TYPE_ID; }

    void Init() override { hp = 100; }
    void Update(q20_12_t dt) override {}
    void Destroy() override {}

    // Broadcast an event to the global bus
    void TakeDamage(int amount) {
        Event::Damage msg;
        msg.amount = amount;
        ae::BroadcastEvent(msg);
    }

    // React to events from Systems
    void on_receive(const Event::Damage& msg) {
        hp -= msg.amount;
    }

    // CRTP fallback (Do NOT use 'override' here)
    void on_receive_unknown(const etl::imessage&) {}

protected:
    void SubmitToManager() override {}

private:
    int hp;
};

```

### 2. Configure and Run the Engine

```cpp
#include <aegis/engine.hpp>

// Statically define all memory boundaries for your project
using GameEngine = ae::Engine<
    sizeof(HealthComponent),   // Largest Component Size
    alignof(HealthComponent),  // Largest Component Alignment
    256,                       // Max Entities
    1024                       // Max Components
>;

// Implement hardware-specific wrappers
void NDS_PollInput() {
    // scanKeys();
}

void NDS_Compute() {
    // swiWaitForVBlank();
}

int main()
{
    GameEngine engine;

    // Inject hardware hooks and register singletons
    engine.SetPollInputCallback(&NDS_PollInput);
    engine.SetComputeCallback(&NDS_Compute);
    engine.SetPollingEnabled(true);
    engine.SetComputeEnabled(true);

    engine.InitAll();

    // Spawn entities and attach components
    ae::Entity* player = engine.CreateEntity();
    if (player != nullptr) {
        player->AddComponent(engine.CreateComponent<HealthComponent>());
    }

    // Core Loop
    while (true) {
        // dt = 1/60th of a second
        engine.Tick(ae::q20_12_t(1) / 60);
    }

    engine.ShutdownAll();
    return 0;
}

```

> See additional examples under /examples

## Configuration

All capacity constraints (e.g., maximum entities, maximum components per entity, maximum routers) are strictly defined as `constexpr` defaults within `<aegis/types.hpp>`. You can override these defaults per-project by explicitly passing template arguments when instantiating your `ae::Engine`.

---

![Alt](https://repobeats.axiom.co/api/embed/688e10f06f887746a68124a2d64b2da43f99ff19.svg "Repobeats analytics image")

---

## Legal
### License
The codebase is licensed under the Creative Commons **Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0).**

For the codebase, the license means:

- **Attribution (BY)**: You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
- **NonCommercial (NC)**: You may not utilize this codebase for commercial purposes.
- **ShareAlike (SA)**: If you remix, transform, or build upon the engine code, you must distribute your contributions under the same license as the original.

### Libraries
**[ETL](https://github.com/etlcpp/etl)**
- Copyright © 2014–2026, Embedded Template Library
- Licensed under the MIT License

**[fpm](https://github.com/MikeLankamp/fpm)**
- Copyright © 2019, Mike Lankamp
- Licensed under the MIT License

If you want to use the codebase in a commercial application, contact thep3dproject@gmail.com for solutions.
