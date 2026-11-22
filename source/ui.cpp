/**
 * @file    ui.cpp
 * @brief   ...
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

#include "ui.hpp"

void ui(entt::registry& registry)
{
    // uiRoot.draw(registry);

    UIButton button(0, 0, 3, 5, []() { printf("button clicked!"); });

    button.draw(registry);
}
