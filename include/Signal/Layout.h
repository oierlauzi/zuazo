#pragma once

#include "PadBase.h"
#include "SourcePad.h"
#include "ConsumerPad.h"

#include <set>

namespace Zuazo::Signal {

class Layout {
public:
    Layout() = default;
    Layout(const Layout& other) = delete;
    Layout(Layout&& other) = delete;
    virtual ~Layout() = default;

    const std::set<PadBase*>& getOutputs();  
    const std::set<PadBase*>& getInputs(); 

    template<typename T>
    SourcePad<T>*           getOutput();
    template<typename T>
    SourcePad<T>*           getOutput(const std::string& str);
    template<typename T>
    const SourcePad<T>*     getOutput() const;
    template<typename T>
    const SourcePad<T>*     getOutput(const std::string& str) const;

    template<typename T>
    ConsumerPad<T>*         getInput();
    template<typename T>
    ConsumerPad<T>*         getInput(const std::string& str);
    template<typename T>
    const ConsumerPad<T>*   getInput() const; 
    template<typename T>
    const ConsumerPad<T>*   getInput(const std::string& str) const;           
protected:
    template<typename T>
    void                    addOutput(SourcePad<T>& src);
    template<typename T>
    void                    removeOutput(SourcePad<T>& src);

    template<typename T>
    void                    addInput(ConsumerPad<T>& cons);
    template<typename T>
    void                    removeInput(ConsumerPad<T>& cons);
private:
    std::set<PadBase*>      m_outputs;
    std::set<PadBase*>      m_inputs;
};

}

#include "Layout.inl"