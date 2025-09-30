#include "BasePlatformInterface.h"

WindowData::WindowData(size_t id, const std::wstring& title, const std::wstring& className, int x, int y, int w, int h, int zOrder) :
	id(id), title(title), className(className), x(x), y(y), w(w), h(h), zOrder(zOrder)
{
}

WindowData::WindowData(size_t id, std::wstring&& title, std::wstring&& className, int x, int y, int w, int h, int zOrder) :
	id(id), title(std::move(title)), className(std::move(className)), x(x), y(y), w(w), h(h), zOrder(zOrder)
{
}
