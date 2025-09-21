enum class MouseButton
{
    Left,
    Right,
    Middle
};

class BasePlatformInterface
{
public:
    virtual ~BasePlatformInterface() = default;

    virtual bool getMouseButtonPressed(MouseButton button) const = 0;
    virtual void getGlobalMousePosition(int& x, int& y) const = 0;

    virtual void getScreenResolution(int& w, int& h) const = 0;
};