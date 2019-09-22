#include "PadBase.h"

#include <utility>

namespace Zuazo::Signal {

inline PadBase::PadBase(std::string&& name) :
    m_name(std::forward<std::string>(name))
{
} 

inline const std::string& PadBase::getName() const{
    return m_name;
}

inline void PadBase::setName(std::string&& name){
    m_name = std::forward<std::string>(name);
}

}