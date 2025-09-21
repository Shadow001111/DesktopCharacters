#pragma once
#include <windows.h>
#include "BaseWindow.h"

// WindowsWindow inherits from BaseWindow
class WindowsWindow : public BaseWindow
{
public:
    WindowsWindow();
    ~WindowsWindow();

    // Override createWindow from BaseWindow
    bool createWindow(int width, int height, const wchar_t* title) override;

    // Override runLoop from BaseWindow
    int runLoop() override;

private:
    // Window class name
    const wchar_t* className;

    // Handle to the window
    HWND hwnd;

    // Handle to the application instance
    HINSTANCE hInstance;

    // Static window procedure callback
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Instance method for handling messages
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
};
