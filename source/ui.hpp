/**
 * @file    ui.hpp
 * @brief   ...
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

#include "components_generated.h"
#include <cstdio>
#include <entt/entt.hpp>
#include <etl/span.h>
#include <functional>
#include <string>

inline void printIndent(int spaces)
{
    for (int i = 0; i < spaces; i++)
    {
        printf(" ");
    }
}

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

    virtual ~UIWidget() = default;

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
        : UIButton(x, y, length, width, "Button", onClick)
    {
    }

    UIButton(int x, int y, int length, int width, const std::string& label, std::function<void()> onClick)
        : UIWidget(x, y), length_(length), width_(width), label_(label), onClick_(onClick)
    {
    }

    // Container constructor
    UIButton(int x, int y, etl::span<UIWidget*> children, int length, int width, std::function<void()> onClick)
        : UIButton(x, y, children, length, width, "Button", onClick)
    {
    }

    UIButton(int x,
             int y,
             etl::span<UIWidget*> children,
             int length,
             int width,
             const std::string& label,
             std::function<void()> onClick)
        : UIWidget(x, y, children), length_(length), width_(width), label_(label), onClick_(onClick)
    {
    }

  protected:
    void onDraw(entt::registry& registry) override
    {
        printIndent(x_);
        printf("[Button: %s]", label_.c_str());
        printf("\n");

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
    std::string label_;
};

class UILabel : public UIWidget
{
  public:
    UILabel(int x, int y, const std::string& text) : UIWidget(x, y), text_(text)
    {
    }

  protected:
    void onDraw(entt::registry& registry) override
    {
        (void)registry;
        printIndent(x_);
        printf("[Label: %s]\n", text_.c_str());
    }

  private:
    std::string text_;
};

class UIPanel : public UIWidget
{
  public:
    UIPanel(int x, int y, const std::string& title, etl::span<UIWidget*> children)
        : UIWidget(x, y, children), title_(title)
    {
    }

  protected:
    void onDraw(entt::registry& registry) override
    {
        (void)registry;
        printIndent(x_);
        printf("[Panel: %s]\n", title_.c_str());
    }

  private:
    std::string title_;
};
