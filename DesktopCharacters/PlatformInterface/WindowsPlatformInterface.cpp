#include "WindowsPlatformInterface.h"
#include <windows.h>

bool WindowsPlatformInterface::getMouseButtonPressed(MouseButton button) const
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

void WindowsPlatformInterface::getGlobalMousePosition(int& x, int& y) const
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

void WindowsPlatformInterface::getScreenResolution(int& w, int& h) const
{
    w = GetSystemMetrics(SM_CXSCREEN);
    h = GetSystemMetrics(SM_CYSCREEN);
}
