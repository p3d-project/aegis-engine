/**
 * @file    main.cpp
 * @brief   Example showing how to use EnTT
 * @note    Example based from https://github.com/skypjack/entt#code-example
 * @author  Taha Rashid (TheBossT910 / thebosst)
 */

#include "components.hpp"
#include "scene.hpp"
#include "systems.hpp"
#include <entt/entt.hpp>

// main program execution
int main()
{
    entt::registry registry;

    // create entities
    for (auto i = 0u; i < 10u; ++i)
    {
        const auto entity = registry.create();
        registry.emplace<position>(entity, i * 1.f, i * 1.f);
        registry.emplace<velocity>(entity, i * .1f, i * .1f);
    }

    // run systems
    for (int i = 0; i < 10; i++)
    {
        // scene
        session();

        // systems
        exampleSystem(registry);

        // bridge
        // bridgeECSToUI();

        // ui
        // ui();
    }

    printf("done!");
}
