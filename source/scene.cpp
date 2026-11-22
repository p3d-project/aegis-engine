/**
 * @file    scene.cpp
 * @brief   "Views" in old aegis engine
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

#include "scene.hpp"
#include "components_generated.h"
#include "tags_generated.h"
#include <cstdint>
#include <fstream>
#include <iterator>
#include <vector>

void loadScene(entt::registry& registry, const Scenes::SceneDefT& sceneDef)
{
    unloadScene(registry);

    // set data
    if (sceneDef.scene_data)
    {
        registry.ctx().emplace<Scenes::SceneDataT>(*sceneDef.scene_data);
    }
    if (sceneDef.graphics_data)
    {
        registry.ctx().emplace<Scenes::GraphicsDataT>(*sceneDef.graphics_data);
    }

    // add entities
    for (const auto& sceneEntity : sceneDef.entities)
    {
        const entt::entity entity = registry.create();
        registry.emplace<Tags::SceneEntityTagT>(entity);

        // add components
        if (sceneEntity->position)
        {
            registry.emplace<Components::PositionT>(entity, *sceneEntity->position);
        }
        if (sceneEntity->velocity)
        {
            registry.emplace<Components::VelocityT>(entity, *sceneEntity->velocity);
        }
        if (sceneEntity->input)
        {
            registry.emplace<Components::InputT>(entity, *sceneEntity->input);
        }
    }
}

void loadScene(entt::registry& registry, const char* binaryPath)
{
    std::ifstream file(binaryPath, std::ios::binary);
    // TODO: replace std::vector with etl::vector
    const std::vector<std::uint8_t> buffer{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

    flatbuffers::Verifier verifier(buffer.data(), buffer.size());
    if (!file || !Scenes::VerifySceneDefBuffer(verifier))
    {
        return;
    }

    std::unique_ptr<Scenes::SceneDefT> sceneDef(Scenes::UnPackSceneDef(buffer.data()));
    loadScene(registry, *sceneDef);
}

void unloadScene(entt::registry& registry)
{
    auto view = registry.view<Tags::SceneEntityTagT>();
    registry.destroy(view.begin(), view.end());
    registry.ctx().erase<Scenes::SceneDataT>();
    registry.ctx().erase<Scenes::GraphicsDataT>();
}
