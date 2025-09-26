#pragma once
#include "BasePlatformInterface.h"

class Windows_PlatformInterface : public BasePlatformInterface
{
public:
    void start() override;

    bool getMouseButtonPressed(MouseButton button) const override;
    void getGlobalMousePosition(int& x, int& y) const override;

    void getScreenResolution(int& w, int& h) const override;

    void getWindowsDataForCharacters(std::vector<WindowData>& result) const override;
};

