#include "Windows_Renderer.h"

#include <stdexcept>

Windows_Renderer::Windows_Renderer(HWND hwnd) :
    hwnd(hwnd), factory(nullptr), renderTarget(nullptr),
    dwriteFactory(nullptr), textFormat(nullptr)
{
    // Direct 2D
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);

    // DirectWrite
    DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&dwriteFactory)
    );

    // Default text format
    if (dwriteFactory)
    {
        dwriteFactory->CreateTextFormat(
            L"Segoe UI",                // Font family
            nullptr,                    // Font collection
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            48.0f,                      // Font size
            L"en-us",                   // Locale
            &textFormat
        );
    }
}

Windows_Renderer::~Windows_Renderer()
{
    discardResources();
    if (textFormat) textFormat->Release();
    if (dwriteFactory) dwriteFactory->Release();
    if (factory) factory->Release();
}

void Windows_Renderer::beforeRender()
{
    createResources();

    renderTarget->BeginDraw();
    renderTarget->Clear(D2D1::ColorF(0, 0, 0, 0));
}

void Windows_Renderer::afterRender()
{
    HRESULT hr = renderTarget->EndDraw();

    if (hr == D2DERR_RECREATE_TARGET)
    {
        // Device was lost (e.g., GPU reset, driver update, window moved to another GPU, etc.)
        discardResources();
    }
    else if (FAILED(hr))
    {
        throw std::runtime_error("RenderTarget->EndDraw failed");
    }
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
    hwndProps.presentOptions = D2D1_PRESENT_OPTIONS_IMMEDIATELY;

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

void Windows_Renderer::drawRectangle(float x, float y, float w, float h, const Color& color, float strokeWidth)
{
    if (!renderTarget || !brush) return;

    brush->SetColor({ color.r, color.g, color.b, color.a });

    D2D1_RECT_F rect = { x, y, x + w, y + h };

    if (strokeWidth <= 0.0f)
    {
        renderTarget->FillRectangle(rect, brush);
    }
    else
    {
        renderTarget->DrawRectangle(rect, brush, strokeWidth);
    }
}

void Windows_Renderer::drawEllipse(float cx, float cy, float rx, float ry, const Color& color, float strokeWidth)
{
    if (!renderTarget || !brush) return;

    brush->SetColor({ color.r, color.g, color.b, color.a });

    D2D1_ELLIPSE ellipse = D2D1::Ellipse(
        D2D1::Point2F(cx, cy),
        rx,
        ry
    );

    if (strokeWidth <= 0.0f)
    {
        renderTarget->FillEllipse(ellipse, brush);
    }
    else
    {
        renderTarget->DrawEllipse(ellipse, brush, strokeWidth);
    }
}

void Windows_Renderer::drawLine(float x1, float y1, float x2, float y2, const Color& color, float strokeWidth)
{
    if (!renderTarget || !brush) return;

    brush->SetColor({ color.r, color.g, color.b, color.a });

    renderTarget->DrawLine(
        D2D1::Point2F(x1, y1),
        D2D1::Point2F(x2, y2),
        brush,
        strokeWidth
    );
}

void Windows_Renderer::drawText(const std::wstring& text, float x, float y, float w, float h, const Color& color)
{
    if (!renderTarget || !brush || !textFormat) return;

    brush->SetColor({ color.r, color.g, color.b, color.a });

    D2D1_RECT_F layoutRect = D2D1::RectF(x, y, x + w, y + h);

    renderTarget->DrawTextW(
        text.c_str(),
        static_cast<UINT32>(text.length()),
        textFormat,
        &layoutRect,
        brush
    );
}
