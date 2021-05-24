#pragma once

#include <string>

namespace Zuazo::Signal {

class Layout;

template<typename T>
class PadProxy;

class PadBase {
public:
	PadBase(const Layout& layout, std::string name) noexcept;
	PadBase(const PadBase& other) = default; 
	PadBase(PadBase&& other) noexcept = default;
	virtual ~PadBase() = default;

	PadBase&								operator=(const PadBase& other) = default;
	PadBase&								operator=(PadBase&& other) noexcept = default;
	
	bool									operator==(const PadBase& other) const noexcept;
	bool									operator!=(const PadBase& other) const noexcept;
	bool									operator<(const PadBase& other) const noexcept;
	bool									operator<=(const PadBase& other) const noexcept;
	bool									operator>(const PadBase& other) const noexcept;
	bool									operator>=(const PadBase& other) const noexcept;

	PadProxy<PadBase>&						getProxy() noexcept;
	const PadProxy<PadBase>&				getProxy() const noexcept;

	void                					setLayout(const Layout& layout) noexcept;
	const Layout&							getLayout() const noexcept;

	void                					setName(std::string name) noexcept;
	const std::string&						getName() const noexcept;
private:
	std::reference_wrapper<const Layout>	m_layout;
	std::string								m_name;
	
};



template<>
class PadProxy<PadBase>
	: PadBase
{
	friend PadBase;
	friend Layout;
public:
	PadProxy() = delete;
	PadProxy(const PadProxy& other) = delete;

	PadProxy& 						operator=(const PadProxy& other) = delete;

	using PadBase::operator==;
	using PadBase::operator!=;
	using PadBase::operator<;
	using PadBase::operator<=;
	using PadBase::operator>;
	using PadBase::operator>=;
	using PadBase::getLayout;
	using PadBase::getName;

private:
	~PadProxy() = default;
	
};

}

#include "PadBase.inl"