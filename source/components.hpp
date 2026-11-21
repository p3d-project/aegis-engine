/**
 * @file    components.hpp
 * @brief   Example showing how to make components
 * @note    Example based from https://github.com/skypjack/entt#code-example
 * @author  Taha Rashid (TheBossT910 / thebosst)
 */

// components are structs
#include <string>
#include <tuple>

/**
 * @brief ...
 */
struct SceneEntityTag
{
};

/**
 * @brief ...
 */
struct SceneData
{
    std::string name;
};

/**
 * @brief ...
 */
struct GraphicsData
{
    // video mode
    // TODO: use enums
    int videoSetMode = -1;
    int videoSetModeSub = -1;

    // vram
    // TODO: use enums
    int vramSetBankA = -1;
    int vramSetBankB = -1;
    int vramSetBankC = -1;
    int vramSetBankD = -1;
    int vramSetBankE = -1;
    int vramSetBankF = -1;
    int vramSetBankG = -1;
    int vramSetBankH = -1;
    int vramSetBankI = -1;

    // extended backgrounds
    bool bgExtPaletteEnable = false;
    bool bgExtPaletteEnableSub = false;

    // screen brightness
    std::tuple<int, int> setBrightness;

    // background
    // TODO: replace with etl tuple
    std::tuple<int, int, int, int, int> bgInit[4];
    std::tuple<int, int, int, int, int> bgInitSub[4];
    std::tuple<int, int> bgSetPriority[8];
};

/**
 * @brief ...
 */
struct SceneEntity
{
    std::string name;

    // TODO: list of defined components
    // ...
    // Position{...}
    // Velocity{...}
};

/**
 * @brief ...
 */
struct SceneDef
{
    int id;
    SceneData sceneData;
    GraphicsData graphicsData;
    // list of scene entities
    SceneEntity* sceneEntities;
};

/**
 * @brief Example
 */
struct Position
{
    float x;
    float y;
};

/**
 * @brief Example
 */
struct Velocity
{
    float dx;
    float dy;
};
