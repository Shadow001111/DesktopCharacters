#pragma once
#include <windows.h>
#include "BaseWindow.h"

// WindowsWindow inherits from BaseWindow
class WindowsWindow : public BaseWindow
{
public:
    WindowsWindow();
    ~WindowsWindow();

    bool createWindow(const WindowParams& params) override;

    int runLoop() override;

private:
    // Window class name
    const wchar_t* className;

    // Handle to the window
    HWND hwnd;

    // Handle to the application instance
    HINSTANCE hInstance;

    // Store parameters for reference
    WindowParams windowParams;

    // Static window procedure callback
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Instance method for handling messages
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
};
