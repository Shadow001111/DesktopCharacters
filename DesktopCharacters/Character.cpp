#include "Character.h"

#include "Window/WindowsWindow.h"

#include <iostream>

using WindowClass = WindowsWindow;

// Constructor
Character::Character()
    : x(0), y(0), width(400), height(300), alive(false)
{
}

// Destructor
Character::~Character()
{
    destroy();
}

// Create the character with a window
bool Character::create(const WindowParams& params)
{
    if (window != nullptr)
    {
        std::cout << "Character is already created!" << std::endl;
        return false;
    }

    // Create the window with provided parameters
    window = std::make_unique<WindowClass>();
    if (!window->createWindow(params))
    {
        std::cout << "Failed to create character window!" << std::endl;
        window.reset();
        return false;
    }

    // Set character properties from window parameters
    x = 0; // Windows will position automatically if CW_USEDEFAULT
    y = 0;
    width = params.width;
    height = params.height;
    alive = true;

    std::cout << "Character created at (" << x << "," << y << ") size " << width << "x" << height << std::endl;
    return true;
}

// Set character position
void Character::setPosition(int newX, int newY)
{
    if (!alive || !window) return;

    x = newX;
    y = newY;

    // Move the actual window
    WindowClass* winWindow = static_cast<WindowClass*>(window.get());
    if (winWindow && winWindow->isValid())
    {
        HWND hwnd = winWindow->getHWND();
        if (hwnd)
        {
            SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            std::cout << "Character moved to (" << x << "," << y << ")" << std::endl;
        }
    }
}

// Set character size
void Character::setSize(int newWidth, int newHeight)
{
    if (!alive || !window) return;

    width = newWidth;
    height = newHeight;

    // Resize the actual window
    WindowClass* winWindow = static_cast<WindowClass*>(window.get());
    if (winWindow && winWindow->isValid())
    {
        HWND hwnd = winWindow->getHWND();
        if (hwnd)
        {
            SetWindowPos(hwnd, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
            std::cout << "Character resized to " << width << "x" << height << std::endl;
        }
    }
}

// Move character by delta
void Character::move(int deltaX, int deltaY)
{
    setPosition(x + deltaX, y + deltaY);
}

// Resize character
void Character::resize(int newWidth, int newHeight)
{
    setSize(newWidth, newHeight);
}

int Character::getX() const
{
    return x;
}

int Character::getY() const
{
    return y;
}

int Character::getWidth() const
{
    return width;
}

int Character::getHeight() const
{
    return height;
}

// Check if character is alive
bool Character::isAlive() const
{
    if (!alive || !window) return false;

    // Check if underlying window is still valid
    WindowClass* winWindow = static_cast<WindowClass*>(window.get());
    return winWindow && winWindow->isValid();
}

// Destroy the character
void Character::destroy()
{
    if (window)
    {
        window.reset();
        std::cout << "Character destroyed" << std::endl;
    }
    alive = false;
}

BaseWindow* Character::getWindow() const
{
    return window.get();
}
