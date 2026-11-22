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

    UIWidget* startChildren[] = {&startStatus};
    etl::span<UIWidget*> startSpan(startChildren);
    UIButton start(2, 1, startSpan, 1, 12, "Start", []() { printf("  [action: start clicked]\n"); });

    UIWidget* panelChildren[] = {&title, &start};
    etl::span<UIWidget*> panelSpan(panelChildren);
    UIPanel panel(0, 0, "Main Menu", panelSpan);

    UIButton quit(0, 2, 1, 12, "Quit", []() { printf("  [action: quit clicked]\n"); });

    UIWidget* rootChildren[] = {&panel, &quit};
    etl::span<UIWidget*> rootSpan(rootChildren);
    UIWidget uiExample(0, 0, rootSpan);

    uiExample.draw(registry);
}
