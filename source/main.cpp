/**
 * @file    main.cpp
 * @brief   Example showing how to use EnTT
 * @note    Example based from https://github.com/skypjack/entt#code-example
 * @author  Taha Rashid (TheBossT910 / thebosst)
 */

#include "scene.hpp"
#include "systems/systems.hpp"
#include "ui.hpp"
#include <entt/entt.hpp>

// TODO: make an example UI

// main program execution
int main()
{
    printf("start!\n\n");

    entt::registry registry;

    // load initial scene
    loadScene(registry, SCENES_BINARY_PATH);

    // update loop
    for (int i = 0; i < 10; i++)
    {
        // run systems
        inputSystem(registry);
        exampleSystem(registry);
        renderSystem(registry);

        // renders UI
        // TODO: to switch the UI being rendered, we need to switch the ui root to another "root"
        ui(registry);
    }

    printf("done!\n\n");
}
