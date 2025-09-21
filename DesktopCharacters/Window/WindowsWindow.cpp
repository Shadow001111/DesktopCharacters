#include "WindowsWindow.h"
#include <stdexcept>

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
bool WindowsWindow::createWindow(int width, int height, const wchar_t* title)
{
    // Fill WNDCLASS structure
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowsWindow::windowProc; // Set static callback
    wc.hInstance = hInstance;                 // Current instance handle
    wc.lpszClassName = className;                 // Window class name
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // Default arrow cursor

    // Register window class
    if (!RegisterClass(&wc))
    {
        return false;
    }

    // Create the actual window
    hwnd = CreateWindowEx(
        0,                 // No extended styles
        className,         // Window class name
        title,             // Window title
        WS_OVERLAPPEDWINDOW, // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, width, height, // Position and size
        nullptr, nullptr, hInstance, this // Pass this pointer as lParam
    );

    if (!hwnd)
    {
        return false;
    }

    // Show and update the window
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
