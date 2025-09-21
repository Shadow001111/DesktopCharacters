#pragma once
#include <functional>

enum class WindowEventType
{
    None,
    Close,          // Window closed
    Resize,         // Resized
    MouseDown,      // Mouse button pressed
    MouseUp,        // Mouse button released
    MouseMove,      // Mouse moved
    KeyDown,        // Key pressed
    KeyUp,          // Key released
    FocusGained,    // Window gained focus
    FocusLost       // Window lost focus
};

struct WindowEvent
{
    WindowEventType type = WindowEventType::None;

    // Position info
    int localMouseX = 0;
    int localMouseY = 0;

    // Button or key code
    int keyCode = 0;

    // Resize info
    int width = 0;
    int height = 0;
};

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
    using EventCallback = std::function<void(const WindowEvent&)>;

    virtual ~BaseWindow() = default;

    virtual bool createWindow(const WindowParams& params) = 0;
    virtual int runLoop() = 0;
    virtual bool isValid() const = 0;

    void setCallback(EventCallback cb);
protected:
    EventCallback callback;
};
