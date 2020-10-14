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
	
	explicit Layout(std::string name, std::initializer_list<PadRef> pads = {});
	Layout(const Layout& other) = default;
	Layout(Layout&& other) = default;
	virtual ~Layout() = default;

	Layout&													operator=(const Layout& other) = default;
	Layout&													operator=(Layout&& other) = default;

	void                    								setName(std::string name) noexcept;
	const std::string&      								getName() const noexcept;

	template<typename T>
	std::vector<std::reference_wrapper<PadProxy<T>>>		getPads();
	template<typename T>
	std::vector<std::reference_wrapper<const PadProxy<T>>>	getPads() const;

	template<typename T>
	PadProxy<T>&											getPad(std::string_view name);
	template<typename T>
	const PadProxy<T>&										getPad(std::string_view name) const;

	template<typename T>
	static PadProxy<T>&										makeProxy(T& pad) noexcept;
	template<typename T>
	static const PadProxy<T>&								makeProxy(const T& pad) noexcept;

protected:
	void													registerPad(PadRef pad);
	template<typename T>
	void													registerPad(PadProxy<T>& pad);
	template<typename InputIt>
	void													registerPad(InputIt begin, InputIt end);
	void													removePad(PadRef pad) noexcept;

private:
	std::string												m_name;
	std::vector<PadRef>										m_pads;

	template<typename T>
	T&														findPad(std::string_view name) const;
	template<typename T>
	std::vector<std::reference_wrapper<T>>					findPads() const;
	
};

}

#include "Layout.inl"