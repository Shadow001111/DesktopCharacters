#include "Windows_PlatformInterface.h"

#include <windows.h>

#include <unordered_set>
#include <iostream>

const std::unordered_set<std::wstring> BANNED_CLASS_NAMES =
{
    L"DesktopCharacters_Shadow001111",
    L"Windows.UI.Core.CoreWindow",
    L"ApplicationFrameWindow"
};

static wchar_t wcharBuffer[256];
static int g_orderCounter = 0;
static std::vector<WindowData>* windowDataCollectionVector = nullptr;

BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam)
{
    // Check visibility
    if (!IsWindowVisible(hwnd))
        return TRUE;

    // Skip minimized windows
    if (IsIconic(hwnd))
        return TRUE;

    // Skip child windows (only top-level windows)
    if (GetParent(hwnd))
        return TRUE;

    // Skip owned windows (tool/dialogs usually)
    if (GetWindow(hwnd, GW_OWNER))
        return TRUE;

    // Check extended window styles
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW)
        return TRUE;

    // Get window rect
    RECT rect;
    if (!GetWindowRect(hwnd, &rect))
        return TRUE;

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    if (width == 0 || height == 0)
        return TRUE;

    // Get title
    GetWindowTextW(hwnd, wcharBuffer, 256);
    std::wstring wtitle = wcharBuffer;
    if (wtitle.empty())
        return TRUE;

    // Get class name
    GetClassNameW(hwnd, wcharBuffer, 256);
    std::wstring wclassName = wcharBuffer;

    if (BANNED_CLASS_NAMES.find(wclassName) != BANNED_CLASS_NAMES.end())
        return TRUE;

    // Construct WindowData
    windowDataCollectionVector->emplace_back
    (
        (size_t)hwnd,
        std::move(wtitle),
        std::move(wclassName),
        rect.left,
        rect.top,
        width,
        height,
        g_orderCounter++
    );

    return TRUE;
}


void Windows_PlatformInterface::start()
{
    
}

bool Windows_PlatformInterface::getMouseButtonPressed(MouseButton button) const
{
    int vkCode = 0;
    switch (button)
    {
    case MouseButton::Left:
        vkCode = VK_LBUTTON;
        break;
    case MouseButton::Right:
        vkCode = VK_RBUTTON;
        break;
    case MouseButton::Middle:
        vkCode = VK_MBUTTON;
        break;
    }
    return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
}

void Windows_PlatformInterface::getGlobalMousePosition(int& x, int& y) const
{
    POINT pt;
    if (GetCursorPos(&pt))
    {
        x = pt.x;
        y = pt.y;
        return;
    }
    x = 0;
    y = 0;
}

void Windows_PlatformInterface::getScreenResolution(int& w, int& h) const
{
    w = GetSystemMetrics(SM_CXSCREEN);
    h = GetSystemMetrics(SM_CYSCREEN);
}

void Windows_PlatformInterface::getWindows(std::vector<WindowData>& result) const
{
    g_orderCounter = 0;
    windowDataCollectionVector = &result;
    EnumWindows(EnumWindowsCallback, 0);
}
