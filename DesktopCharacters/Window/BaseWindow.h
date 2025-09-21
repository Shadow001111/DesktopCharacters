#pragma once

// Struct holding parameters for window creation
struct WindowParams
{
    int width;                 // Width of the window
    int height;                // Height of the window
    const wchar_t* title;      // Window title
    bool topMost = false;              // Should the window stay on top
    bool frameless = false;            // Should the window be frameless
    bool fullscreen = false;           // Fill entire screen
    bool ignoreMouse = false;          // Do not receive mouse clicks or drags
};

// Base class containing only pure virtual methods
class BaseWindow
{
public:
    virtual ~BaseWindow() = default;

    // Method to create the window
    virtual bool createWindow(const WindowParams& params) = 0;

    // Method to run the message loop
    virtual int runLoop() = 0;
};
