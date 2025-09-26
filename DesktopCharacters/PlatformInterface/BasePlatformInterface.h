#include <vector>
#include <functional>
#include <string>

enum class MouseButton
{
    Left,
    Right,
    Middle
};

struct WindowData
{
    std::wstring title;
    std::wstring className;
    int x, y, w, h;
    int zOrder;
};

class BasePlatformInterface
{
public:
    virtual ~BasePlatformInterface() = default;

    virtual void start() = 0;

    virtual bool getMouseButtonPressed(MouseButton button) const = 0;
    virtual void getGlobalMousePosition(int& x, int& y) const = 0;

    virtual void getScreenResolution(int& w, int& h) const = 0;
    
    virtual void getWindowsDataForCharacters(std::vector<WindowData>& result) const = 0;
};