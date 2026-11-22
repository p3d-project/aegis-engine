/**
 * @file    ui.cpp
 * @brief   ...
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

#include "ui.hpp"

void ui(entt::registry& registry)
{
    UILabel title(2, 0, "AEGIS Engine");
    UILabel startStatus(4, 0, "Ready");
    UIButton start(2, 1, 1, 12, "Start", []() { printf("  [action: start clicked]\n"); });
    UIPanel panel(0, 0, "Main Menu");
    UIButton quit(0, 2, 1, 12, "Quit", []() { printf("  [action: quit clicked]\n"); });
    UIWidget uiExample(0, 0);

    uiExample.addChild(panel.addChild(title).addChild(start.addChild(startStatus))).addChild(quit);

    uiExample.draw(registry);
}
