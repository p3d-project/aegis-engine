/**
 * @file    systems.cpp
 * @brief   Example showing how to make systems
 * @note    Example based from https://github.com/skypjack/entt#code-example
 * @author  Taha Rashid (TheBossT910 / thebosst)
 */

#include "systems.hpp"
#include "components_generated.h"
#include "scenes_generated.h"

void exampleSystem(entt::registry& registry)
{
    for (const entt::entity entity : registry.view<entt::entity>())
    {
        if (const auto* pos = registry.try_get<const Components::PositionT>(entity))
        {
            printf("pos:x - %f\n", pos->x);
            printf("pos:y - %f\n", pos->y);
        }

        if (const auto* vel = registry.try_get<const Components::VelocityT>(entity))
        {
            printf("vel:dx - %f\n", vel->dx);
            printf("vel:dy - %f\n", vel->dy);
        }
    }

    printf("exampleSystem: running\n\n");
}

void renderSystem(entt::registry& registry)
{
    // GraphicsDataT is stored as registry context data
    // ...
    printf("renderSystem: running\n\n");
}

void inputSystem(entt::registry& registry)
{
    // ...
    printf("inputSystem: running\n\n");
}
