#include "Layout.h"

namespace Zuazo::Signal {

const std::set<PadBase*>& Layout::getOutputs(){
    return m_outputs;
}

const std::set<PadBase*>& Layout::getInputs();{
    return m_inputs;
}

template<typename T>
inline SourcePad<T>* Layout::getOutput(){
    for(PadBase* pad : m_outputs){
        //Test if it has the correct type
        auto ptr = dynamic_cast<SourcePad<T>*>(pad);
        if(ptr){
            return ptr;
        }
    }

    return nullptr; //Did not find anything
}

template<typename T>
inline SourcePad<T>* Layout::getOutput(const std::string& str){
    for(PadBase* pad : m_outputs){
        if(pad->getName() == str){
            //Found a element with the correct name
            //Test if it has the correct type
            auto ptr = dynamic_cast<SourcePad<T>*>(pad);
            if(ptr){
                return ptr;
            }
        }
    }

    return nullptr; //Did not find anything
}

template<typename T>
inline const SourcePad<T>* Layout::getOutput() const{
    for(const PadBase* pad : m_outputs){
        //Test if it has the correct type
        auto ptr = dynamic_cast<const SourcePad<T>*>(pad);
        if(ptr){
            return ptr;
        }
    }

    return nullptr; //Did not find anything
}

template<typename T>
inline const SourcePad<T>* Layout::getOutput(const std::string& str) const{
    for(const PadBase* pad : m_outputs){
        if(pad->getName() == str){
            //Found a element with the correct name
            //Test if it has the correct type
            auto ptr = dynamic_cast<const SourcePad<T>*>(pad);
            if(ptr){
                return ptr;
            }
        }
    }

    return nullptr; //Did not find anything
}

template<typename T>
inline ConsumerPad<T>* Layout::getInput(){
    for(PadBase* pad : m_inputs){
        //Test if it has the correct type
        auto ptr = dynamic_cast<ConsumerPad<T>*>(pad);
        if(ptr){
            return ptr;
        }
    }

    return nullptr; //Did not find anything
}

template<typename T>
inline ConsumerPad<T>* Layout::getInput(const std::string& str){
    for(PadBase* pad : m_inputs){
        if(pad->getName() == str){
            //Found a element with the correct name
            //Test if it has the correct type
            auto ptr = dynamic_cast<ConsumerPad<T>*>(pad);
            if(ptr){
                return ptr;
            }
        }
    }

    return nullptr; //Did not find anything
}

template<typename T>
inline const ConsumerPad<T>* Layout::getInput() const{
    for(const PadBase* pad : m_inputs){
        //Test if it has the correct type
        auto ptr = dynamic_cast<const ConsumerPad<T>*>(pad);
        if(ptr){
            return ptr;
        }
    }

    return nullptr; //Did not find anything
}

template<typename T>
inline const ConsumerPad<T>* Layout::getInput(const std::string& str) const{
    for(const PadBase* pad : m_inputs){
        if(pad->getName() == str){
            //Found a element with the correct name
            //Test if it has the correct type
            auto ptr = dynamic_cast<const ConsumerPad<T>*>(pad);
            if(ptr){
                return ptr;
            }
        }
    }

    return nullptr; //Did not find anything
}              

template<typename T>
inline void Layout::addOutput(SourcePad<T>& src){
    m_outputs.emplace(&src);
}

template<typename T>
inline void Layout::removeOutput(SourcePad<T>& src){
    m_outputs.erase(&src);
}

template<typename T>
inline void Layout::addInput(ConsumerPad<T>& cons){
    m_inputs.emplace(&cons);
}

template<typename T>
inline void Layout::removeInput(ConsumerPad<T>& cons){
    m_inputs.erase(&cons);
}

}