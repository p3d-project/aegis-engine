/**
 * @file    main.cpp
 * @brief   Example showing how to use EnTT
 * @note    Example from https://github.com/skypjack/entt#code-example
 * @author  Taha Rashid (TheBossT910 / thebosst)
 */

#include "components.hpp"
#include <entt/entt.hpp>

// update loop
void update(entt::registry& registry)
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
        auto& vel = view.get<velocity>(entity);
        // ...
    }
}

// main program exicution
int main()
{
    entt::registry registry;

    for (auto i = 0u; i < 10u; ++i)
    {
        const auto entity = registry.create();
        registry.emplace<position>(entity, i * 1.f, i * 1.f);
        if (i % 2 == 0)
        {
            registry.emplace<velocity>(entity, i * .1f, i * .1f);
        }
    }

    // simulate update loop
    for (int i = 0; i < 10; i++)
    {
        update(registry);
    }

    printf("done!");
}
