#pragma once
#include "Window/BaseWindow.h"

#include <memory>

// Character represents a desktop character entity
class Character
{
public:
    Character();
    ~Character();

    // Create the character with a window
    bool create(const WindowParams& params);

    // Character properties
    void setPosition(int x, int y);
    void setSize(int width, int height);
    void move(int deltaX, int deltaY);
    void resize(int newWidth, int newHeight);

    // Get character properties
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;

    // Character state
    bool isAlive() const;
    void destroy();

    // Window access (for manager)
    BaseWindow* getWindow() const;
private:
    std::unique_ptr<BaseWindow> window;

    // Character position and size
    int x, y;
    int width, height;

    // Character state
    bool alive;
};