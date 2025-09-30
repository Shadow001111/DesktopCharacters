#include <vector>
#include <string>

enum class MouseButton
{
    Left,
    Right,
    Middle
};

struct WindowData
{
    size_t id = 0;
    std::wstring title;
    std::wstring className;
    int x = 0, y = 0, w = 0, h = 0;
    int zOrder = 0;

    WindowData() = default;
    WindowData(size_t id, const std::wstring& title, const std::wstring& className, int x, int y, int w, int h, int zOrder);
    WindowData(size_t id, std::wstring&& title, std::wstring&& className, int x, int y, int w, int h, int zOrder);
};

class BasePlatformInterface
{
public:
    virtual ~BasePlatformInterface() = default;

    virtual void start() = 0;

    virtual bool getMouseButtonPressed(MouseButton button) const = 0;
    virtual void getGlobalMousePosition(int& x, int& y) const = 0;

    virtual void getScreenResolution(int& w, int& h) const = 0;
    
    virtual void getWindows(std::vector<WindowData>& result) const = 0;
};