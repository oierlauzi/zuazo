#include <Zuazo.h>

#include <Timing/MainThread.h>

namespace Zuazo {

void init(){
    Timing::MainThread::init();
}

void end(){
    Timing::MainThread::end();
}

}