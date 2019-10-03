#include <Timing/LazyEvent.h>

namespace Zuazo::Timing {

void LazyEvent::setMaximumRecursion(uint rec){
    m_maxRecursion = rec;
}

uint LazyEvent::getMaximumRecursion() const {
    return m_maxRecursion;
}


void LazyEvent::evaluate() {
    if(isEnabled()){
        if(m_recursion <  m_maxRecursion){
            m_recursion++;
            update();
            m_recursion--;
        }
    }
}

}