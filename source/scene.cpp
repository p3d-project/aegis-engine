/**
 * @file    scene.cpp
 * @brief   "Views" in old aegis engine
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

#include "scene.hpp"
#include <etl/stack.h>

// TODO: SceneDef - the actual defined view. has data on entity setup, view loading, etc. init(), update(), cleanup() overrides

constexpr int stackSize = 4;

etl::stack<Scene, stackSize> activeStack;
etl::stack<Scene, stackSize> initStack;
etl::stack<Scene, stackSize> cleanupStack;

void push(Scene& scene)
{
    initStack.push(scene);
    activeStack.push(scene);
}

void pop()
{
    cleanupStack.push(etl::move(activeStack.top()));
    activeStack.pop();
}

void session()
{
    // shared data
    // ...

    // init new scenes
    while (!initStack.empty())
    {
        Scene& scene = initStack.top();
        scene.init();
        initStack.pop();
    }

    // update active scene
    if (!activeStack.empty())
    {
        Scene& scene = activeStack.top();
        scene.update();
    }

    // cleanup old scenes
    while (!cleanupStack.empty())
    {
        Scene& scene = cleanupStack.top();
        scene.cleanup();
        cleanupStack.pop();
    }
}
