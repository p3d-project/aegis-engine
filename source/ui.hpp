/**
 * @file    ui.hpp
 * @brief   ...
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

#include "components_generated.h"
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

inline UIWidget uiRoot{0, 0};

/**
 * @brief Draws the UI root
 * @param registry EnTT registry
 */
void ui(entt::registry& registry);

class UIButton : public UIWidget
{
  public:
    // Leaf constructor
    UIButton(int x, int y, int length, int width, std::function<void()> onClick)
        : UIWidget(x, y), length_(length), width_(width), onClick_(onClick)
    {
    }

    // Container constructor
    UIButton(int x, int y, etl::span<UIWidget*> children, int length, int width, std::function<void()> onClick)
        : UIWidget(x, y, children), length_(length), width_(width), onClick_(onClick)
    {
    }

  protected:
    void onDraw(entt::registry& registry) override
    {
        // draw a button
        char border = '-';
        std::string line(width_, border);

        for (int i = 0; i < length_; i++)
        {
            printf("%s\n", line.c_str());
        }

        auto view = registry.view<Components::InputT>();
        for (auto [entity, input] : view.each())
        {
            if (input.key == 1)
            {
                onClick_();
            }
        }
    }

  private:
    std::function<void()> onClick_;
    int length_, width_;
    int colorId;
};
