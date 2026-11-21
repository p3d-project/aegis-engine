/**
 * @file    systems.hpp
 * @brief   Systems header file
 * @author  Taha Rashid (TheBossT910 / thebosst)
 */

#include <entt/entt.hpp>

/**
 * @brief Example system
 * @param registry EnTT registry
 */
void exampleSystem(entt::registry& registry);

/**
 * @brief Renders graphics
 * @param registry EnTT registry
 */
void renderSystem(entt::registry& registry);

/**
 * @brief Polls inputs
 * @param registry EnTT registry
 */
void inputSystem(entt::registry& registry);
