/**
 * @file    ui.cpp
 * @brief   ...
 * @author  Taha Rashid (TheBossT910 / thebosst)
**/

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

    void draw()
    {
        onDraw();

        for (UIWidget* child : children_)
        {
            if (child != nullptr)
            {
                child->draw();
            }
        }
    }

  protected:
    int x_, y_;

    // override to implement functionality
    virtual void onDraw()
    {
    }

  private:
    etl::span<UIWidget*> children_;
};
