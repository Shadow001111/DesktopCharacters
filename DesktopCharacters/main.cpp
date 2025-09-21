#include "Window/WindowsWindow.h"

#include <iostream>
#include <fstream>

void openConsole()
{
    // Create a new console
    AllocConsole();

    // Redirect stdout, stdin, stderr to the new console
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    std::cout << "Debug console is ready!" << std::endl;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _DEBUG
    openConsole();
#endif

    // Create instance of WindowsWindow
    WindowsWindow window;
    
    WindowParams params;
    params.width = 800;
    params.height = 600;
    params.title = L"(-_-)";
    //params.topMost = true;
    //params.frameless = true;
    //params.fullscreen = true;
    //params.ignoreMouse = true;

    // Create a window 800x600 with title
    if (!window.createWindow(params))
    {
        return -1;
    }

    // Run the message loop
    return window.runLoop();
}
