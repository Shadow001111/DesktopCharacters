#include "Windows_Renderer.h"
#include <stdexcept>

Windows_Renderer::Windows_Renderer(HWND hwnd)
    : hwnd(hwnd), factory(nullptr), renderTarget(nullptr),
    pendingShape(ShapeType::None), lineStroke(2.0f)
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
}

void Windows_Renderer::discardResources()
{
    if (renderTarget)
    {
        renderTarget->Release();
        renderTarget = nullptr;
    }
}

void Windows_Renderer::render()
{
    createResources();

    renderTarget->BeginDraw();
    renderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

    if (pendingShape != ShapeType::None)
    {
        ID2D1SolidColorBrush* brush = nullptr;
        renderTarget->CreateSolidColorBrush(pendingColor, &brush);

        switch (pendingShape)
        {
        case ShapeType::Rect:
            renderTarget->FillRectangle(&rectData, brush);
            break;
        case ShapeType::Ellipse:
            renderTarget->FillEllipse(ellipseData, brush);
            break;
        case ShapeType::Line:
            renderTarget->DrawLine(lineStart, lineEnd, brush, lineStroke);
            break;
        default:
            break;
        }

        if (brush) brush->Release();
    }

    HRESULT hr = renderTarget->EndDraw();
    if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        discardResources();
}

void Windows_Renderer::drawRectangle(float x, float y, float w, float h, const Color& color)
{
    pendingShape = ShapeType::Rect;
    pendingColor = D2D1::ColorF(color.r, color.g, color.b, color.a);
    rectData = D2D1::RectF(x, y, x + w, y + h);
    InvalidateRect(hwnd, nullptr, FALSE);
}

void Windows_Renderer::drawEllipse(float cx, float cy, float rx, float ry, const Color& color)
{
    pendingShape = ShapeType::Ellipse;
    pendingColor = D2D1::ColorF(color.r, color.g, color.b, color.a);
    ellipseData = D2D1::Ellipse(D2D1::Point2F(cx, cy), rx, ry);
    InvalidateRect(hwnd, nullptr, FALSE);
}

void Windows_Renderer::drawLine(float x1, float y1, float x2, float y2, const Color& color, float strokeWidth)
{
    pendingShape = ShapeType::Line;
    pendingColor = D2D1::ColorF(color.r, color.g, color.b, color.a);
    lineStart = D2D1::Point2F(x1, y1);
    lineEnd = D2D1::Point2F(x2, y2);
    lineStroke = strokeWidth;
    InvalidateRect(hwnd, nullptr, FALSE);
}
