#include <Zuazo.h>

#include <Timing/Loop.h>

namespace Zuazo {

void init(){
    Timing::mainLoop = std::make_unique<Timing::Loop>();
}

void end(){
    Timing::mainLoop.reset();
}

}