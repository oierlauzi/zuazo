#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <utility>


namespace Zuazo::Signal {

class PadBase;

class NoSignal {};
constexpr NoSignal noSignal;

class Layout {
	friend PadBase;
public:
	template<typename T>
	class PadProxy;

	using PadSet = std::vector<std::unique_ptr<PadBase>>;

	template<typename Str, typename Pads>
	Layout(Str&& name, Pads&& pads);
	Layout(const Layout& other) = delete;
	Layout(Layout&& other) = default;
	virtual ~Layout() = default;

	Layout&                 	operator=(const Layout& other) = delete;
	Layout&                 	operator=(Layout&& other) = default;

	template<typename Str>
	void                    	setName(Str&& name);
	const std::string&      	getName() const;

	template<typename T>
	std::vector<PadProxy<T>>	getPads();

	template<typename T>
	std::vector<const PadProxy<T>> getPads() const;

	template<typename T>
	PadProxy<T>					getPad(std::string_view str);

	template<typename T>
	const PadProxy<T>			getPad(std::string_view str) const;

protected:
	template <typename Pad>
	void						addPad(Pad&& pad);
	void						removePad(PadBase& pad);

	template <typename T>
	T&							findPad(std::string_view str) const;

	template <typename T>
	std::vector<std::reference_wrapper<T>>	findPads() const;

private:
	std::string					m_name;
	PadSet						m_pads;

};

}

#include "Layout.inl"