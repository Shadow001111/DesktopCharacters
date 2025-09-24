#include "Windows_Renderer.h"

#include <stdexcept>
#include <iostream>

Windows_Renderer::Windows_Renderer(HWND hwnd)
    : hwnd(hwnd), factory(nullptr), renderTarget(nullptr)
{
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
}

Windows_Renderer::~Windows_Renderer()
{
    discardResources();
    if (factory) factory->Release();
}

void Windows_Renderer::createResources()
{
    // Render traget
    if (renderTarget) return;

    RECT clientRect = {};
    GetClientRect(hwnd, &clientRect);

    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    D2D1_RENDER_TARGET_PROPERTIES rtProps = {};
    rtProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
        D2D1_ALPHA_MODE_PREMULTIPLIED);
    rtProps.usage = D2D1_RENDER_TARGET_USAGE_NONE;
    rtProps.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

    HDC hdc = GetDC(hwnd);

    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = {};
    hwndProps.hwnd = hwnd;
    hwndProps.pixelSize = D2D1::SizeU(width, height);
    hwndProps.presentOptions = D2D1_PRESENT_OPTIONS_NONE;

    HRESULT hr = factory->CreateHwndRenderTarget(rtProps, hwndProps, &renderTarget);
    ReleaseDC(hwnd, hdc);

    if (FAILED(hr))
        throw std::runtime_error("Failed to create render target");

    // Brush
    hr = renderTarget->CreateSolidColorBrush(
        D2D1::ColorF(0, 0, 0), &brush
    );
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create brush");
    }
}

void Windows_Renderer::discardResources()
{
    if (renderTarget)
    {
        renderTarget->Release();
        renderTarget = nullptr;
    }
    if (brush)
    {
        brush->Release();
        brush = nullptr;
    }
}

// Called on WM_PAINT
void Windows_Renderer::render()
{
    createResources();

    renderTarget->BeginDraw();
    renderTarget->Clear(D2D1::ColorF(0, 0, 0, 0));

    for (const auto& s : pendingShapes)
    {
        brush->SetColor(s.color);

        switch (s.type)
        {
        case ShapeType::Rect:
            renderTarget->FillRectangle(&s.rect, brush);
            break;
        case ShapeType::Ellipse:
            renderTarget->FillEllipse(s.ellipse, brush);
            break;
        case ShapeType::Line:
            renderTarget->DrawLine(s.start, s.end, brush, s.stroke);
            break;
        default:
            break;
        }
    }

    pendingShapes.clear();

    HRESULT hr = renderTarget->EndDraw();
    if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        discardResources();
}

void Windows_Renderer::drawRectangle(float x, float y, float w, float h, const Color& color)
{
    Shape s{ ShapeType::Rect, D2D1::ColorF(color.r, color.g, color.b, color.a) };
    s.rect = D2D1::RectF(x, y, x + w, y + h);
    pendingShapes.push_back(s);

    InvalidateRect(hwnd, nullptr, FALSE); // TODO: pass the rect as pointer
}

void Windows_Renderer::drawEllipse(float cx, float cy, float rx, float ry, const Color& color)
{
    Shape s{ ShapeType::Ellipse, D2D1::ColorF(color.r, color.g, color.b, color.a) };

    s.ellipse = D2D1::Ellipse(D2D1::Point2F(cx, cy), rx, ry);

    pendingShapes.push_back(s);
    InvalidateRect(hwnd, nullptr, FALSE);
}

void Windows_Renderer::drawLine(float x1, float y1, float x2, float y2, const Color& color, float strokeWidth)
{
    Shape s{ ShapeType::Line, D2D1::ColorF(color.r, color.g, color.b, color.a) };

    s.start = D2D1::Point2F(x1, y1);
    s.end = D2D1::Point2F(x2, y2);
    s.stroke = strokeWidth;

    pendingShapes.push_back(s);
    InvalidateRect(hwnd, nullptr, FALSE);
}
