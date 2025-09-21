#include "Window/WindowsWindow.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // Create instance of WindowsWindow
    WindowsWindow window;

    // Create a window 800x600 with title
    if (!window.createWindow(800, 600, L"DesktopCharacters"))
    {
        return -1;
    }

    // Run the message loop
    return window.runLoop();
}
