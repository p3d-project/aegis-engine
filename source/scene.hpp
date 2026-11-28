/**
 * @file    scene.hpp
 * @brief   Defines scene functions
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

#pragma once
#include "scenes_generated.h"
#include <entt/entt.hpp>
#include <etl/string_view.h>

/**
 * @brief Load scene into ECS
 */
void loadScene(entt::registry& registry, const Scenes::SceneDefT& sceneDef);
void loadScene(entt::registry& registry, etl::string_view binaryPath);

/**
 * @brief Unload scene from ECS
 */
void unloadScene(entt::registry& registry);
