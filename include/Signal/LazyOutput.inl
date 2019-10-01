#include "LazyOutput.h"

namespace Zuazo::Signal {

template <typename T>
LazyOutput<T>::LazyOutput(Timing::LazyEvent& evnt, std::string&& name) :
    Output<T>(std::forward<std::string>(name)),
    m_owner(m_owner)
{
}

template <typename T>
std::shared_ptr<const T>& LazyOutput<T>::get() const{
    m_owner.evaluate();
    return Output<T>::get();
}

}