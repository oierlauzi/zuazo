#pragma once

#include "Graphics/Context.h"
#include "Timing.h"

namespace Zuazo{

inline void begin(){
    Timing::getTiming()->lock();
    Graphics::Context::getMainCtx()->use();
}

inline void end(){
    Graphics::Context::getMainCtx()->unuse();
    Timing::getTiming()->unlock();
}

class Context{
public:
    Context();
    Context(const Context& other)=delete;
    Context(Context&& context)=delete;
    ~Context();
};

inline Context::Context(){
    begin();
}

inline Context::~Context(){
    end();
}

}