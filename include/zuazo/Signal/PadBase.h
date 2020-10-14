#pragma once

#include "Layout.h"

#include <string>

namespace Zuazo::Signal {

class PadBase {
public:
	explicit PadBase(std::string name) noexcept;
	PadBase(const PadBase& other) = default; 
	PadBase(PadBase&& other) noexcept = default;
	virtual ~PadBase() = default;

	PadBase&						operator=(const PadBase& other) = default;
	PadBase&						operator=(PadBase&& other) noexcept = default;
	
	bool							operator==(const PadBase& other) const noexcept;
	bool							operator!=(const PadBase& other) const noexcept;
	bool							operator<(const PadBase& other) const noexcept;
	bool							operator<=(const PadBase& other) const noexcept;
	bool							operator>(const PadBase& other) const noexcept;
	bool							operator>=(const PadBase& other) const noexcept;

	void                			setName(std::string name) noexcept;
	const std::string&				getName() const noexcept;

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

	PadProxy& 						operator=(const PadProxy& other) = delete;

	using PadBase::operator==;
	using PadBase::operator!=;
	using PadBase::operator<;
	using PadBase::operator<=;
	using PadBase::operator>;
	using PadBase::operator>=;
	using PadBase::getName;

private:
	~PadProxy() = default;
	
};

}

#include "PadBase.inl"