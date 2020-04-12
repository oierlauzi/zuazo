#include "Layout.h"

#include "PadBase.h"
#include "../Exception.h"

#include <type_traits>
#include <cassert>
#include <algorithm>

namespace Zuazo::Signal {

/*
 * Layout
 */

template<typename Str, typename... Pads>
inline Layout::Layout(Str&& name, Pads&&... pads)
	: m_name(std::forward<Str>(name))
	, m_pads{std::make_unique<Pads>(std::forward<Pads>(pads))...}
{
}

template<typename Str>
inline void Layout::setName(Str&& name) {
	m_name = std::forward<Str>(name);
}

inline const std::string& Layout::getName() const{
	return m_name;
}



template<typename T>
inline std::vector<Layout::PadProxy<T>> Layout::getPads() {
	const auto vec = findPads<T>();
	return std::vector<PadProxy<T>>(vec.cbegin(), vec.cend());
}

template<typename T>
inline std::vector<const Layout::PadProxy<T>>	Layout::getPads() const {
	const auto vec = findPads<T>();
	return std::vector<const PadProxy<T>>(vec.cbegin(), vec.cend());
}

template<typename T>
inline Layout::PadProxy<T> Layout::getPad(std::string_view str) {
	return findPad<T>(str);
}

template<typename T>
inline const Layout::PadProxy<T> Layout::getPad(std::string_view str) const {
	return findPad<T>(str);
}


template <typename Pad>
inline void Layout::addPad(Pad&& pad){
	m_pads.emplace_back(std::make_unique<Pad>(std::forward<Pad>(pad))); 
}

inline void Layout::removePad(PadBase& pad){
	const auto cond = [=] (const std::unique_ptr<PadBase> ptr) -> bool {
		return ptr.get() == &pad;
	};

	//Remove it from my array
	auto ite = std::remove_if(m_pads.begin(), m_pads.end(), cond);
	m_pads.erase(ite, m_pads.cend());
}


template <typename T>
inline T& Layout::findPad(std::string_view str) const {
	static_assert(std::is_base_of<PadBase, T>::value, "T must inherit from PadBase");
	for(const auto& pad : m_pads) {
		if(pad->getName() == str){
			auto* p = dynamic_cast<T*>(pad.get());
			if(p) return *p;
		}
	}

	throw Exception("Requested pad not found");
}

template <typename T>
inline std::vector<std::reference_wrapper<T>> Layout::findPads() const {
	static_assert(std::is_base_of<PadBase, T>::value, "T must inherit from PadBase");
	std::vector<T> result;

	for(const auto& pad : m_pads) {
		auto* p = dynamic_cast<T*>(pad.get());
		if(p) result.emplace_back(*p);
	}

	return result;
}

}