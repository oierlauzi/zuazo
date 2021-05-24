#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <utility>

namespace Zuazo::Signal {

class PadBase;

template<typename T>
class PadProxy;

class Layout {
public:
	using PadRef = std::reference_wrapper<PadBase>;
	
	explicit Layout(std::string name);
	template<typename InputIt>
	Layout(std::string name, InputIt beginPads, InputIt endPads);
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
	PadProxy<T>*											getPad(std::string_view name) noexcept;
	template<typename T>
	const PadProxy<T>*										getPad(std::string_view name) const noexcept;

protected:
	void													registerPad(PadRef pad);
	template<typename T>
	void													registerPad(PadProxy<T>& pad);
	template<typename InputIt>
	void													registerPad(InputIt begin, InputIt end);

	void													removePad(PadRef pad) noexcept;
	template<typename T>
	void													removePad(PadProxy<T>& pad);

private:
	std::string												m_name;
	std::vector<PadRef>										m_pads;

	template<typename T>
	T*														findPad(std::string_view name) const noexcept;
	template<typename T>
	std::vector<std::reference_wrapper<T>>					findPads() const;
	
};

}

#include "Layout.inl"