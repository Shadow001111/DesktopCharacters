#include "BaseWindow.h"

void BaseWindow::setCallback(EventCallback cb)
{
    callback = std::move(cb);
}

BaseRenderer* BaseWindow::getRenderer() const
{
    return renderer.get();
}
