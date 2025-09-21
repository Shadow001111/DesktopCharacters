#include "WindowsWindow.h"

#include <stdexcept>
#include <iostream>

// Constructor
WindowsWindow::WindowsWindow()
    : className(L"MyWindowClass"), hwnd(nullptr), hInstance(GetModuleHandle(nullptr))
{
}

// Destructor
WindowsWindow::~WindowsWindow()
{
    // If a window was created, destroy it
    if (hwnd)
    {
        DestroyWindow(hwnd);
    }
}

// Create window
bool WindowsWindow::createWindow(const WindowParams& params)
{
    windowParams = params;

    // Fill WNDCLASS
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowsWindow::windowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClass(&wc))
    {
        return false;
    }

    // Base style
    DWORD style = WS_OVERLAPPEDWINDOW;

    // Extended styles
    DWORD exStyle = 0;

    // Adjust for fullscreen
    int winX = CW_USEDEFAULT;
    int winY = CW_USEDEFAULT;
    int winW = params.width;
    int winH = params.height;

    // Create window
    hwnd = CreateWindowEx(
        exStyle,
        className,
        params.title,
        style,
        winX, winY, winW, winH,
        nullptr, nullptr, hInstance, this
    );

    if (!hwnd)
    {
        return false;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    return true;
}

// Main message loop
int WindowsWindow::runLoop()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);  // Translate keyboard input
        DispatchMessage(&msg);   // Dispatch message to windowProc
    }
    return static_cast<int>(msg.wParam);
}

// Static window procedure
LRESULT CALLBACK WindowsWindow::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    WindowsWindow* window = nullptr;

    // On WM_NCCREATE, store pointer to the class in window user data
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<WindowsWindow*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        window->hwnd = hwnd;
    }
    else
    {
        // Retrieve pointer to class from window user data
        window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window)
    {
        return window->handleMessage(msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Instance method handling messages
LRESULT WindowsWindow::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0); // Post quit message
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
