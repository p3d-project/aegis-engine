/**
 * @file    ui.hpp
 * @brief   ...
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

#include <entt/entt.hpp>
#include <etl/span.h>

class UIWidget
{
  public:
    // Leaf constructor
    UIWidget(int x, int y) : x_(x), y_(y), children_()
    {
    }

    // Container constructor
    UIWidget(int x, int y, etl::span<UIWidget*> children) : x_(x), y_(y), children_(children)
    {
    }

    /**
     * @brief Draws the UIWidget and any children
     * @param registry EnTT registry
     */
    void draw(entt::registry& registry)
    {
        onDraw(registry);

        for (UIWidget* child : children_)
        {
            if (child != nullptr)
            {
                child->draw(registry);
            }
        }
    }

  protected:
    int x_, y_;

    /**
     * @brief Draw the UIWidget
     * @note override to implement functionality
     * @param registry EnTT registry
     */
    virtual void onDraw(entt::registry& registry)
    {
    }

  private:
    etl::span<UIWidget*> children_;
};

UIWidget uiRoot;

/**
 * @brief Draws the UI root
 * @param registry EnTT registry
 */
void ui(entt::registry& registry);
