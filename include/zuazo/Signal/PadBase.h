#pragma once

#include "Layout.h"

#include <string>

namespace Zuazo::Signal {

class PadBase {
	friend Layout;
public:
	PadBase(std::string name);
	PadBase(const PadBase& other) = default; 
	PadBase(PadBase&& other) = default;
	virtual ~PadBase() = default;

	PadBase&						operator=(const PadBase& other) = default;
	PadBase&						operator=(PadBase&& other) = default;
	
	bool							operator==(const PadBase& other) const;
	bool							operator!=(const PadBase& other) const;
	bool							operator<(const PadBase& other) const;
	bool							operator<=(const PadBase& other) const;
	bool							operator>(const PadBase& other) const;
	bool							operator>=(const PadBase& other) const;

	void                			setName(std::string name);
	const std::string&				getName() const;

private:
	std::string						m_name;
	
};

template<>
class Layout::PadProxy<PadBase>
	: PadBase
{
public:
	friend Layout;
	
	PadProxy() = delete;
	PadProxy(const PadProxy& other) = delete;
	~PadProxy() = default;

	PadProxy& 						operator=(const PadProxy& other) = delete;

	using PadBase::operator==;
	using PadBase::operator!=;
	using PadBase::operator<;
	using PadBase::operator<=;
	using PadBase::operator>;
	using PadBase::operator>=;
	using PadBase::getName;
};

}

#include "PadBase.inl"