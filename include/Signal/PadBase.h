#pragma once

#include "Layout.h"

#include <string>

namespace Zuazo::Signal {

class Layout;

class PadBase {
	friend Layout;
public:
	PadBase() = default;
	template<typename Str>
	PadBase(Str&& name);
	PadBase(const PadBase& other) = default; 
	PadBase(PadBase&& other) = default;
	virtual ~PadBase() = default;

	PadBase&					operator=(const PadBase& other) = default;
	PadBase&					operator=(PadBase&& other) = default;

	template<typename Str>
	void                		setName(Str&& name);
	const std::string&			getName() const;

private:
	std::string					m_name;
	
};

template<>
class Layout::PadProxy<PadBase> {
public:
	PadProxy(PadBase& pad);
	PadProxy(const PadProxy& other) = delete;
	~PadProxy() = default;

	PadProxy& operator=(const PadProxy& other) = delete;

	const std::string&			getName() const;

protected:
	template <typename T>
	T&							get();

	template <typename T>
	const T&					get() const;

private:
	PadBase&					m_pad;
};

}

#include "PadBase.inl"