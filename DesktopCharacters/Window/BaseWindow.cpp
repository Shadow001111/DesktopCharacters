#include "BaseWindow.h"

void BaseWindow::setCallback(EventCallback cb)
{
    callback = std::move(cb);
}