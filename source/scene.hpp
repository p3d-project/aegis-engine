/**
 * @file    scene.hpp
 * @brief   Header file for scene
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

class Scene
{
  public:
    std::string name;

    void init();
    void update();
    void cleanup();
};

/**
 * @brief Push a new Scene onto the stack
 * @note Exposed to Scene
 * @param scene The Scene to push
 */
void push(Scene& scene);

/**
 * @brief Pop the currently active Scene (at the top of the stack)
 * @note Exposed to Scene
 */
void pop();

/**
 * @brief Manages Scenes
 */
void session();
