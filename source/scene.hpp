/**
 * @file    scene.hpp
 * @brief   ...
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

#include "scenes_generated.h"
#include <entt/entt.hpp>

/**
 * @brief ...
 */
void loadScene(entt::registry& registry, const Scenes::SceneDefT& sceneDef);
void loadScene(entt::registry& registry, const char* binaryPath);

/**
 * @brief ...
 */
void unloadScene(entt::registry& registry);
