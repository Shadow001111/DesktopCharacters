#pragma once
#include "BasePlatformInterface.h"

class WindowsPlatformInterface : public BasePlatformInterface
{
public:
    bool getMouseButtonPressed(MouseButton button) const override;
    void getGlobalMousePosition(int& x, int& y) const override;

    void getScreenResolution(int& w, int& h) const override;

    void getWindowsDataForCharacters(std::vector<WindowData>& result) const override;
};

