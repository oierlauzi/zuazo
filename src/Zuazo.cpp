#include <Zuazo.h>

#include <Timing/MainThread.h>

namespace Zuazo {

void init(){
    Timing::mainThread = std::make_unique<Timing::MainThread>(); //Create the main thread
}

void end(){
    Timing::mainThread.reset();
}

void _setup(){

}

void _cleanup(){
    
}

}