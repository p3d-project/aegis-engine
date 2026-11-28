/**
 * @file    ui.cpp
 * @brief   UI implementation
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

#include "ui.hpp"

void ui(entt::registry& registry)
{
    UIWidget title(2, 0, UILabel("AEGIS Engine"));
    UIWidget startStatus(4, 0, UILabel("Ready"));
    UIWidget start(
        2,
        1,
        UIButton(1, 12, "Start", etl::delegate<void()>::create(+[]() { printf("  [action: start clicked]\n"); })));
    UIWidget panel(0, 0, UIPanel("Main Menu"));
    UIWidget quit(
        0, 2, UIButton(1, 12, "Quit", etl::delegate<void()>::create(+[]() { printf("  [action: quit clicked]\n"); })));
    UIWidget uiExample(0, 0, UIRoot());

    uiExample.addChild(panel.addChild(title).addChild(start.addChild(startStatus))).addChild(quit);

    uiExample.draw(registry);
}
