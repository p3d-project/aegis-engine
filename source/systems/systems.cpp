/**
 * @file    systems.cpp
 * @brief   Example showing how to make systems
 * @note    Example based from https://github.com/skypjack/entt#code-example
 * @author  Taha Rashid (TheBossT910 / thebosst)
 */

#include "systems.hpp"
#include "components.hpp"

void exampleSystem(entt::registry& registry)
{
    auto view = registry.view<const position, velocity>();

    // use a callback
    view.each([](const auto& pos, auto& vel) { /* ... */ });

    // use an extended callback
    view.each([](const auto entity, const auto& pos, auto& vel) { /* ... */ });

    // use a range-for
    for (auto [entity, pos, vel] : view.each())
    {
        // ...
    }

    // use forward iterators and get only the components of interest
    for (auto entity : view)
    {
        auto& pos = view.get<position>(entity);
        auto& vel = view.get<velocity>(entity);
        // ...
    }
}
