#pragma once

#include "Resolution.h"

#include <string>

namespace Zuazo::Graphics {

class Window {
public:
    Window();
    Window(const Resolution& resolution, const std::string& name);
    Window(Window&& other);
    virtual ~Window();

    void bind();
    void unbind();

private:

};

}