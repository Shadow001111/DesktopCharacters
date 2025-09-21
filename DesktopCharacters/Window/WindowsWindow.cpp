#include "WindowsWindow.h"

#include <stdexcept>
#include <iostream>
#include <memory>

// Constructor
WindowsWindow::WindowsWindow()
    : hwnd(nullptr), hInstance(GetModuleHandle(nullptr))
{
    // Generate unique class name using the object's address
    static int classCounter = 0;
    classCounter++;

    // Create unique class name
    uniqueClassName = std::make_unique<wchar_t[]>(50);
    swprintf_s(uniqueClassName.get(), 50, L"MyWindowClass_%d_%p", classCounter, this);
}

// Destructor
WindowsWindow::~WindowsWindow()
{
    // If a window was created, destroy it
    if (hwnd)
    {
        DestroyWindow(hwnd);
    }

    // Unregister the unique window class
    if (uniqueClassName)
    {
        UnregisterClass(uniqueClassName.get(), hInstance);
    }
}

// Create window
bool WindowsWindow::createWindow(const WindowParams& params)
{
    // Fill WNDCLASS with unique class name
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowsWindow::windowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = uniqueClassName.get();
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 0)); // Red background

    if (!RegisterClass(&wc))
    {
        DWORD error = GetLastError();
        std::cout << "RegisterClass failed with error: " << error << std::endl;
        return false;
    }

    // Base style - start with popup for frameless, otherwise overlapped window
    DWORD style = params.frameless ? WS_POPUP : WS_OVERLAPPEDWINDOW;

    // Extended styles
    DWORD exStyle = 0;

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
        uniqueClassName.get(),
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

    // For topMost windows, we can also use SetWindowPos as an alternative/reinforcement
    if (params.topMost)
    {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
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
        // Mark window as invalid by setting hwnd to null
        hwnd = nullptr;
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

// Check if window is still valid
bool WindowsWindow::isValid() const
{
    return hwnd != nullptr && IsWindow(hwnd);
}

HWND WindowsWindow::getHWND() const
{
    return hwnd;
}

void WindowsWindow::setCharacter(Character* char_ptr)
{
    character = char_ptr;
}
