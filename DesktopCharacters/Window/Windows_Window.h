#pragma once
#include "BaseWindow.h"

#include <windows.h>

#include <string>

// WindowsWindow inherits from BaseWindow
class Windows_Window : public BaseWindow
{
public:
    Windows_Window();
    ~Windows_Window();

    bool createWindow(const InitWindowParams& params) override;
    bool isValid() const;

    bool setPositionAndSize(int x, int y, int w, int h) override;

    HWND getHWND() const;
private:
    // Handle to the window
    HWND hwnd;

    // Handle to the application instance
    HINSTANCE hInstance;

    //
    std::wstring className;

    // Static window procedure callback
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Instance method for handling messages
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
};