#include "Layout.h"

#include <utility>

namespace Zuazo::Signal {

inline Layout::Layout(std::string&& name) :
    m_name(std::forward<std::string>(name))
{
}

inline Layout::~Layout() {
    const auto myPads = m_pads;
    for(auto pad : m_pads){
        removePad(*pad);
    }
}

inline void Layout::setName(std::string&& name){
    m_name = std::forward<std::string>(name);
}

inline const std::string& Layout::getName() const{
    return m_name;
}

inline const std::set<PadBase*>& Layout::getPads(){
    return m_pads;
}

inline std::set<const PadBase*>  Layout::getPads() const{
    return std::set<const PadBase*>(m_pads.cbegin(), m_pads.cend());
}

template<typename T>
inline OutputPad<T>* Layout::getOutput(const std::string& str){
    return findOutput<T>(str);
}

template<typename T>
inline const OutputPad<T>* Layout::getOutput(const std::string& str) const{
    return findOutput<T>(str);
}

template<typename T>
inline InputPad<T>* Layout::getInput(const std::string& str){
    return findInput<T>(str);
}

template<typename T>
inline const InputPad<T>* Layout::getInput(const std::string& str) const{
    return findInput<T>(str);
}              

inline void Layout::addPad(PadBase& pad){
    if(pad.m_owner == nullptr){ //Pad can't have an owner
        pad.m_owner = this; //Make me its owner
        m_pads.emplace(&pad); //Add it to my list
    }
}

inline void Layout::removePad(PadBase& pad){
    if(pad.m_owner == this){ //I must be its owner
        pad.m_owner = nullptr;
        m_pads.erase(&pad); //Remove it from my list
    }
}

template<typename T>
OutputPad<T>* Layout::findOutput(const std::string& name) const{
    for(auto pad : m_pads) {
        if( (pad->getDirection() & PadBase::OUTPUT) && 
            pad->getName() == name )
        {
            auto result = dynamic_cast<OutputPad<T>*>(pad);
            if(result) return result;
        }
    }

    return nullptr;
}

template<typename T>
InputPad<T>* Layout::findInput(const std::string& name) const{
    for(auto pad : m_pads) {
        if( (pad->getDirection() & PadBase::INPUT) && 
            pad->getName() == name )
        {
            auto result = dynamic_cast<InputPad<T>*>(pad);
            if(result) return result;
        }
    }

    return nullptr;
}

}