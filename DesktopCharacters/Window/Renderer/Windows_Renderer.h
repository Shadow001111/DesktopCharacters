#pragma once
#include "BaseRenderer.h"

#include <windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")

class Windows_Renderer : public BaseRenderer
{
public:
    Windows_Renderer(HWND hwnd);
    ~Windows_Renderer();

    void beforeRender() override;
    void afterRender() override;

    void drawRectangle(float x, float y, float w, float h, const Color& color, float strokeWidth = -1.0f) override;

    void drawEllipse(float cx, float cy, float rx, float ry, const Color& color, float strokeWidth = -1.0f) override;

    void drawLine(float x1, float y1, float x2, float y2, const Color& color, float strokeWidth = 2.0f) override;

    void drawText(const std::wstring& text, float x, float y, float w, float h, const Color& color) override;

private:
    void createResources();
    void discardResources();

    HWND hwnd;

    ID2D1Factory* factory;
    ID2D1HwndRenderTarget* renderTarget;
    ID2D1SolidColorBrush* brush;

    IDWriteFactory* dwriteFactory;
    IDWriteTextFormat* textFormat;
};
