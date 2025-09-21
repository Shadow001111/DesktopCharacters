#pragma once
#include <windows.h>
#include <windowsx.h> // For GET_X_LPARAM, GET_Y_LPARAM

#include <memory>
#include <functional>

#include "BaseWindow.h"

// WindowsWindow inherits from BaseWindow
class WindowsWindow : public BaseWindow
{
public:
    WindowsWindow();
    ~WindowsWindow();

    bool createWindow(const WindowParams& params) override;
    int runLoop() override;
    bool isValid() const;

    HWND getHWND() const;
private:
    // Unique window class name for each instance
    std::unique_ptr<wchar_t[]> uniqueClassName;

    // Handle to the window
    HWND hwnd;

    // Handle to the application instance
    HINSTANCE hInstance;

    // Static window procedure callback
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Instance method for handling messages
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
};