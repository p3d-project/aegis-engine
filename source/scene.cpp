/**
 * @file    scene.cpp
 * @brief   "Views" in old aegis engine
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

// SceneDefs are going to be auto-generated

#include "scene.hpp"

void loadScene(entt::registry& registry, SceneDef sceneDef)
{
    unloadScene(registry);

    auto& scene = registry.ctx().emplace<SceneData>();
    auto& gfx = registry.ctx().get<GraphicsData>();

    SceneData& sceneData = sceneDef.sceneData;
    GraphicsData& graphicsData = sceneDef.graphicsData;
    SceneEntity* sceneEntities = sceneDef.sceneEntities;

    // set scene data
    scene = sceneData;

    // set graphics
    gfx = graphicsData;

    // set entities
    // loop through sceneEntities and set
    // automatically set anything in sceneEntity with SceneEntityTag
    // ...
}

void unloadScene(entt::registry& registry)
{
    auto view = registry.view<SceneEntityTag>();
    registry.destroy(view.begin(), view.end());
    registry.ctx().erase<SceneData>();
}
