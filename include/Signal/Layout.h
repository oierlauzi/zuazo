#pragma once

#include "PadBase.h"
#include "OutputPad.h"
#include "InputPad.h"

#include <set>
#include <string>

namespace Zuazo::Signal {

class Layout {
    friend PadBase;
public:
    Layout(std::string&& name);
    Layout(const Layout& other) = delete;
    virtual ~Layout();

    void                    setName(std::string&& name);
    const std::string&      getName() const;

    const std::set<PadBase*>& getPads();  
    std::set<const PadBase*>  getPads() const;  

    template<typename T>
    OutputPad<T>*           getOutput(const std::string& str);
    template<typename T>
    const OutputPad<T>*     getOutput(const std::string& str) const;

    template<typename T>
    InputPad<T>*            getInput(const std::string& str);
    template<typename T>
    const InputPad<T>*      getInput(const std::string& str) const;

protected:
    void                    addPad(PadBase& pad);
    void                    removePad(PadBase& pad);

private:
    std::string             m_name;
    std::set<PadBase*>      m_pads;

    template<typename T>
    OutputPad<T>*           findOutput(const std::string& name) const;
    template<typename T>
    InputPad<T>*            findInput(const std::string& name) const;
};

}

#include "Layout.inl"