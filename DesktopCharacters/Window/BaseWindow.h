#pragma once

// Base class containing only pure virtual functions
class BaseWindow
{
public:
    virtual ~BaseWindow() = default;

    // Pure virtual function to create the window
    virtual bool createWindow(int width, int height, const wchar_t* title) = 0;

    // Pure virtual function to run the message loop
    virtual int runLoop() = 0;
};
