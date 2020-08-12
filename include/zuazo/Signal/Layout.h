#pragma once

#include "../Utils/CopiedPtr.h"

#include <string>
#include <string_view>
#include <vector>
#include <utility>

namespace Zuazo::Signal {

class PadBase;

class Layout {
public:
	template<typename T>
	class PadProxy;

	using PadRef = std::reference_wrapper<PadBase>;
	
	Layout(std::string name, std::initializer_list<PadRef> pads = {});
	Layout(const Layout& other) = default;
	Layout(Layout&& other) = default;
	virtual ~Layout() = default;

	Layout&													operator=(const Layout& other) = default;
	Layout&													operator=(Layout&& other) = default;

	void                    								setName(std::string name);
	const std::string&      								getName() const;

	template<typename T>
	std::vector<std::reference_wrapper<PadProxy<T>>>		getPads();
	template<typename T>
	std::vector<std::reference_wrapper<const PadProxy<T>>>	getPads() const;

	template<typename T>
	PadProxy<T>&											getPad(std::string_view name);
	template<typename T>
	PadProxy<T>&											getPad(const T& pad);
	template<typename T>
	const PadProxy<T>&										getPad(std::string_view name) const;
	template<typename T>
	const PadProxy<T>&										getPad(const T& pad) const;

	template<typename T>
	static PadProxy<T>&										makeProxy(T& pad);
	template<typename T>
	static const PadProxy<T>&								makeProxy(const T& pad);

protected:
	void													registerPad(PadRef pad);
	template<typename T>
	void													registerPad(PadProxy<T>& pad);
	void													registerPads(std::initializer_list<PadRef> pads);
	void													removePad(PadRef pad);

private:
	std::string												m_name;
	std::vector<PadRef>										m_pads;

	template<typename T>
	T&														findPad(std::string_view name) const;
	template<typename T>
	T&														findPad(const T& pad) const;
	template<typename T>
	std::vector<std::reference_wrapper<T>>					findPads() const;
	
};

}

#include "Layout.inl"