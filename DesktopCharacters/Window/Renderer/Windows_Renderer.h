#pragma once
#include "BaseRenderer.h"

#include <windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include <memory>
#include <vector>

class Windows_Renderer : public BaseRenderer
{
public:
    Windows_Renderer(HWND hwnd);
    ~Windows_Renderer();

    void render() override;

    void drawRectangle(float x, float y, float w, float h, const Color& color) override;

    void drawEllipse(float cx, float cy, float rx, float ry, const Color& color) override;

    void drawLine(float x1, float y1, float x2, float y2, const Color& color, float strokeWidth = 2.0f) override;

private:
    void createResources();
    void discardResources();

    HWND hwnd;
    ID2D1Factory* factory;
    ID2D1HwndRenderTarget* renderTarget;
    ID2D1SolidColorBrush* brush;

    // Shape data
    enum class ShapeType { None, Rect, Ellipse, Line };

    struct Shape
    {
        ShapeType type;
        D2D1::ColorF color;
        float stroke;
        D2D1_RECT_F rect;
        D2D1_ELLIPSE ellipse;
        D2D1_POINT_2F start;
        D2D1_POINT_2F end;

        Shape(ShapeType t, const D2D1::ColorF& c)
            : type(t), color(c), stroke(1.0f), rect{}, ellipse{}, start{}, end{}
        {}
    };

    std::vector<Shape> pendingShapes;
};
