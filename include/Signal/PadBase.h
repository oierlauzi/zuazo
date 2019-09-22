#pragma once

#include <string>

namespace Zuazo::Signal {

class PadBase {
public:
    PadBase() = default;
    PadBase(std::string&& name); 
    PadBase(const PadBase& other) = default; 
    PadBase(PadBase&& other) = default; 
    virtual ~PadBase() = default;

    const std::string&  getName() const;
protected:
    void                setName(std::string&& name);
private:   
    std::string         m_name;
};

}

#include "PadBase.inl"