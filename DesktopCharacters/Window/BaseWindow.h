#pragma once
#include "Renderer/BaseRenderer.h"

#include <functional>
#include <memory>

struct WindowEvent
{
    enum class Type
    {
        None,
        Close,          // Window closed
        Resize,         // Resized
        LeftMouseDown,  // Left mouse button pressed
        LeftMouseUp,    // Left mouse button released
        MouseMove,      // Mouse moved
        KeyDown,        // Key pressed
        KeyUp,          // Key released
        FocusGained,    // Window gained focus
        FocusLost       // Window lost focus
    };

    Type type = Type::None;

    // Position info
    int localMouseX = 0;
    int localMouseY = 0;

    // Button or key code
    int keyCode = 0;

    // Resize info
    int width = 0;
    int height = 0;
};

struct InitWindowParams
{
    int width;              
    int height;             
    const wchar_t* title;   
    const wchar_t* className;
    bool topMost = false;   
    bool frameless = false;
    bool fullscreen = false;
    bool ignoreMouse = false;
    bool layered = false;
};

// Base class containing only pure virtual methods
class BaseWindow
{
public:
    using EventCallback = std::function<void(const WindowEvent&)>;

    virtual ~BaseWindow() = default;

    virtual bool createWindow(const InitWindowParams& params) = 0;
    virtual bool setPositionAndSize(int x, int y, int w, int h) = 0;

    virtual bool isValid() const = 0;

    void setCallback(EventCallback cb);

    BaseRenderer* getRenderer() const;
protected:
    EventCallback callback;
    std::unique_ptr<BaseRenderer> renderer;
};
