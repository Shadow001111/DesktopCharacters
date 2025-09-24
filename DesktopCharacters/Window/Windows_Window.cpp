#include "Windows_Window.h"
#include "Renderer/Windows_Renderer.h"

#include <stdexcept>
#include <iostream>
#include <memory>

// Constructor
Windows_Window::Windows_Window()
    : hwnd(nullptr), hInstance(GetModuleHandle(nullptr))
{
}

// Destructor
Windows_Window::~Windows_Window()
{
    // If a window was created, destroy it
    if (hwnd)
    {
        DestroyWindow(hwnd);
    }

    // Unregister the unique window class
    if (!className.empty())
    {
        UnregisterClass(className.c_str(), hInstance);
    }
}

// Create window
bool Windows_Window::createWindow(const InitWindowParams& params)
{
    className = params.className;

    // Fill WNDCLASS
    WNDCLASS wc = {};
    wc.lpfnWndProc = Windows_Window::windowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = params.className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;

    if (!RegisterClass(&wc))
    {
        DWORD error = GetLastError();
        std::cout << "RegisterClass failed with error: " << error << std::endl;
        return false;
    }

    // Base style - start with popup for frameless, otherwise overlapped window
    DWORD style = params.frameless ? WS_POPUP : WS_OVERLAPPEDWINDOW;

    // Extended styles
    DWORD exStyle = WS_EX_APPWINDOW;;

    // Apply topMost parameter
    if (params.topMost)
    {
        exStyle |= WS_EX_TOPMOST;
    }

    // Apply ignoreMouse parameter (make window transparent to mouse)
    if (params.ignoreMouse)
    {
        exStyle |= WS_EX_TRANSPARENT;
    }

    if (params.layered)
    {
        exStyle |= WS_EX_LAYERED;
    }

    // Window position and size
    int winX = CW_USEDEFAULT;
    int winY = CW_USEDEFAULT;
    int winW = params.width;
    int winH = params.height;

    // Adjust for fullscreen
    if (params.fullscreen)
    {
        winX = 0;
        winY = 0;
        winW = GetSystemMetrics(SM_CXSCREEN);
        winH = GetSystemMetrics(SM_CYSCREEN);
        style = WS_POPUP; // Fullscreen should always be popup style
    }

    // Create window
    hwnd = CreateWindowEx(
        exStyle,
        params.className,
        params.title,
        style,
        winX, winY, winW, winH,
        nullptr, nullptr, hInstance, this
    );

    if (!hwnd)
    {
        DWORD error = GetLastError();
        std::cout << "CreateWindowEx failed with error: " << error << std::endl;
        return false;
    }

    if (params.layered)
    {
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Create renderer
    renderer = std::make_unique<Windows_Renderer>(hwnd);

    return true;
}

// Main message loop
int Windows_Window::runLoop()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);  // Translate keyboard input
        DispatchMessage(&msg);   // Dispatch message to windowProc
    }
    return static_cast<int>(msg.wParam);
}

//
bool Windows_Window::isValid() const
{
    return hwnd != nullptr && IsWindow(hwnd);
}

bool Windows_Window::setPositionAndSize(int x, int y, int w, int h)
{
    return SetWindowPos(hwnd, nullptr, x, y, w, h,
        SWP_NOZORDER | SWP_NOACTIVATE);
}

HWND Windows_Window::getHWND() const
{
    return hwnd;
}

// Static window procedure
LRESULT CALLBACK Windows_Window::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Windows_Window* window = nullptr;

    // On WM_NCCREATE, store pointer to the class in window user data
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<Windows_Window*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        window->hwnd = hwnd;
    }
    else
    {
        // Retrieve pointer to class from window user data
        window = reinterpret_cast<Windows_Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window)
    {
        return window->handleMessage(msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Instance method handling messages
LRESULT Windows_Window::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (callback)
    {
        WindowEvent evt;

        switch (msg)
        {
        case WM_CLOSE:
            evt.type = WindowEventType::Close;
            break;

        case WM_SIZE:
            evt.type = WindowEventType::Resize;
            evt.width = LOWORD(lParam);
            evt.height = HIWORD(lParam);
            break;

        case WM_LBUTTONDOWN:
            evt.type = WindowEventType::MouseDown;
            evt.localMouseX = GET_X_LPARAM(lParam);
            evt.localMouseY = GET_Y_LPARAM(lParam);
            break;

        case WM_LBUTTONUP:
            evt.type = WindowEventType::MouseUp;
            evt.localMouseX = GET_X_LPARAM(lParam);
            evt.localMouseY = GET_Y_LPARAM(lParam);
            break;

        case WM_MOUSEMOVE:
            evt.type = WindowEventType::MouseMove;
            evt.localMouseX = GET_X_LPARAM(lParam);
            evt.localMouseY = GET_Y_LPARAM(lParam);
            std::cout << 1;
            break;

        case WM_KEYDOWN:
            evt.type = WindowEventType::KeyDown;
            evt.keyCode = static_cast<int>(wParam);
            break;

        case WM_KEYUP:
            evt.type = WindowEventType::KeyUp;
            evt.keyCode = static_cast<int>(wParam);
            break;
        case WM_PAINT:
            if (renderer)
                renderer->render();
            break;
        }

        if (evt.type != WindowEventType::None)
            callback(evt);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
