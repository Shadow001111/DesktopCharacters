#pragma once
#include "BaseRenderer.h"
#include <windows.h>
#include <d2d1.h>
#include <memory>

// Link Direct2D
#pragma comment(lib, "d2d1")

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

    // Shape data (for now just one at a time, can be extended to a queue)
    enum class ShapeType { None, Rect, Ellipse, Line };
    ShapeType pendingShape;
    D2D1_COLOR_F pendingColor;
    D2D1_RECT_F rectData;
    D2D1_ELLIPSE ellipseData;
    D2D1_POINT_2F lineStart, lineEnd;
    float lineStroke;
};
