#pragma once
#include <windows.h>
#include <windowsx.h> // For GET_X_LPARAM, GET_Y_LPARAM

#include <memory>

#include "BaseWindow.h"
#include "Vec2.h"

// Forward declaration to avoid circular include
class Character;

// WindowsWindow inherits from BaseWindow
class WindowsWindow : public BaseWindow
{
public:
    WindowsWindow();
    ~WindowsWindow();

    bool createWindow(const WindowParams& params) override;

    int runLoop() override;

    // Check if window is still valid
    bool isValid() const;

    // Get window handle for character manipulation
    HWND getHWND() const;

    // Set character reference for drag handling
    void setCharacter(Character* char_ptr);
private:
    // Unique window class name for each instance
    std::unique_ptr<wchar_t[]> uniqueClassName;

    // Handle to the window
    HWND hwnd;

    // Handle to the application instance
    HINSTANCE hInstance;

    // Reference to character for drag handling
    Character* character;

    // Static window procedure callback
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // Instance method for handling messages
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
};