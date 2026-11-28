/**
 * @file    ui.hpp
 * @brief   Defines UIWidget
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

#pragma once
#include "components_generated.h"
#include <entt/entt.hpp>
#include <etl/delegate.h>
#include <etl/variant.h>
#include <etl/vector.h>
#include <etl/visitor.h>
#include <string>

constexpr int maxUIWidgetChildren = 5;

/**
 * @brief UI root
 */
struct UIRoot
{
};

/**
 * @brief UI button with click action
 */
struct UIButton
{
    int length_;
    int width_;
    std::string label_;
    etl::delegate<void()> onClick_;
};

/**
 * @brief UI label
 */
struct UILabel
{
    std::string text_;
};

/**
 * @brief UI panel (grouping)
 */
struct UIPanel
{
    std::string title_;
};
using UIWidgetData = etl::variant<UIRoot, UIButton, UILabel, UIPanel>;

/**
 * @brief Helper function to print text indents
 */
inline void printIndent(int spaces)
{
    for (int i = 0; i < spaces; i++)
    {
        printf(" ");
    }
}

/**
 * @brief Base UI widget class
 */
class UIWidget
{
  public:
    // Leaf constructor
    UIWidget(int x, int y, UIWidgetData data) : x_(x), y_(y), data_(data)
    {
    }

    /**
     * @brief Attaches a child UIWidget object
     * @param child UIWidget child
     */
    UIWidget& addChild(UIWidget& child)
    {
        children_.push_back(&child);
        return *this;
    }

    /**
     * @brief Draws the UIWidget and any children
     * @param registry EnTT registry
     */
    void draw(entt::registry& registry)
    {
        // Draws the appropriate UIWidgetData
        etl::visit([this, &registry](auto& widget) { onDraw(registry, widget); }, data_);

        for (UIWidget* child : children_)
        {
            if (child != nullptr)
            {
                child->draw(registry);
            }
        }
    }

  private:
    int x_, y_;
    UIWidgetData data_;
    etl::vector<UIWidget*, maxUIWidgetChildren> children_;

    /**
     * @brief Draw the UIWidget
     * @param registry EnTT registry
     * @param widget UIRoot widget data
     */
    void onDraw(entt::registry& registry, UIRoot& widget)
    {
    }

    /**
     * @brief Draw the UIButton
     * @param registry EnTT registry
     * @param widget UIButton widget data
     */
    void onDraw(entt::registry& registry, UIButton& widget)
    {
        printIndent(x_);
        printf("[Button: %s]", widget.label_.c_str());
        printf("\n");

        auto view = registry.view<Components::Input>();
        for (auto [entity, input] : view.each())
        {
            (void)entity;
            if (input.key() == 1)
            {
                widget.onClick_();
            }
        }
    }

    /**
     * @brief Draw the UIButton
     * @param registry EnTT registry
     * @param widget UILabel widget data
     */
    void onDraw(entt::registry& registry, UILabel& widget)
    {
        (void)registry;
        printIndent(x_);
        printf("[Label: %s]\n", widget.text_.c_str());
    }

    /**
     * @brief Draw the UIButton
     * @param registry EnTT registry
     * @param widget UIPanel widget data
     */
    void onDraw(entt::registry& registry, UIPanel& widget)
    {
        (void)registry;
        printIndent(x_);
        printf("[Panel: %s]\n", widget.title_.c_str());
    }
};

/**
 * @brief Draws the UI root
 * @param registry EnTT registry
 */
void ui(entt::registry& registry);
