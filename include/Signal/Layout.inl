#include "Layout.h"

namespace Zuazo::Signal {

template<typename T>
inline SourcePad<T>* Layout::getOutput(const std::string& str){
    for(PadBase* src : m_sources){
        if(src->getName() == str){
            //Found a element with the correct name
            //Test if it has the correct type
            auto ptr = dynamic_cast<SourcePad<T>*>(src);
            if(ptr){
                return ptr;
            }
        }
    }

    return nullptr; //Did not find anything
}

template<typename T>
inline const SourcePad<T>* Layout::getOutput(const std::string& str) const{
    for(const PadBase* src : m_sources){
        if(src->getName() == str){
            //Found a element with the correct name
            //Test if it has the correct type
            auto ptr = dynamic_cast<const SourcePad<T>*>(src);
            if(ptr){
                return ptr;
            }
        }
    }

    return nullptr; //Did not find anything
}

template<typename T>
inline ConsumerPad<T>* Layout::getInput(const std::string& str){
    for(PadBase* cons : m_consumers){
        if(cons->getName() == str){
            //Found a element with the correct name
            //Test if it has the correct type
            auto ptr = dynamic_cast<ConsumerPad<T>*>(cons);
            if(ptr){
                return ptr;
            }
        }
    }

    return nullptr; //Did not find anything
}

template<typename T>
inline const ConsumerPad<T>* Layout::getInput(const std::string& str) const{
    for(const PadBase* cons : m_consumers){
        if(cons->getName() == str){
            //Found a element with the correct name
            //Test if it has the correct type
            auto ptr = dynamic_cast<const ConsumerPad<T>*>(cons);
            if(ptr){
                return ptr;
            }
        }
    }

    return nullptr; //Did not find anything
}              

template<typename T>
inline void Layout::addOutput(SourcePad<T>& src){
    m_sources.emplace(&src);
}

template<typename T>
inline void Layout::removeOutput(SourcePad<T>& src){
    m_sources.erase(&src);
}

template<typename T>
inline void Layout::addInput(ConsumerPad<T>& cons){
    m_consumers.emplace(&cons);
}

template<typename T>
inline void Layout::removeInput(ConsumerPad<T>& cons){
    m_consumers.erase(&cons);
}

}