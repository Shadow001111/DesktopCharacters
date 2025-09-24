#include "Windows_PlatformInterface.h"

#include <windows.h>
#include <unordered_set>

#include <iostream>

struct Windows_WindowData
{
    HWND hwnd;
    std::wstring title;
    std::wstring className;
    RECT rect;
    int zOrder;
};

const std::unordered_set<std::wstring> BANNED_CLASS_NAMES =
{
    
};

static std::vector<Windows_WindowData> globalTempWindowsWindowsData;
static int g_orderCounter = 0;

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
    wchar_t title[512];
    GetWindowTextW(hwnd, title, 512);
    std::wstring wtitle = title;
    if (wtitle.empty())
        return TRUE;

    // Get class name
    wchar_t className[256];
    GetClassNameW(hwnd, className, 256);
    std::wstring wclassName = className;

    if (BANNED_CLASS_NAMES.find(wclassName) != BANNED_CLASS_NAMES.end())
        return TRUE;

    // Construct WindowData
    Windows_WindowData wd;
    wd.hwnd = hwnd;
    wd.title = wtitle;
    wd.className = wclassName;
    wd.rect = rect;
    wd.zOrder = g_orderCounter++;

    globalTempWindowsWindowsData.push_back(wd);

    return TRUE;
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

void Windows_PlatformInterface::getWindowsDataForCharacters(std::vector<WindowData>& result) const
{
    globalTempWindowsWindowsData.clear();
    g_orderCounter = 0;
    EnumWindows(EnumWindowsCallback, 0);

    result.reserve(result.size() + globalTempWindowsWindowsData.size());
    for (const auto& wd : globalTempWindowsWindowsData)
    {
        WindowData data;
        data.title = wd.title;
        data.className = wd.className;
        data.x = wd.rect.left;
        data.y = wd.rect.top;
        data.w = wd.rect.right - wd.rect.left;
        data.h = wd.rect.bottom - wd.rect.top;
        data.zOrder = wd.zOrder;
        result.push_back(data);
    }
}
