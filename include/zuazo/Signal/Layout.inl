#include "Layout.h"

#include "PadBase.h"
#include "../Exception.h"

#include <type_traits>
#include <cassert>
#include <algorithm>
#include <tuple>

namespace Zuazo::Signal {

/*
 * Layout
 */

inline Layout::Layout(std::string name, std::initializer_list<PadRef> pads)
	: m_name(std::move(name))
	, m_pads(std::move(pads))
{
}

inline void Layout::setName(std::string name) {
	m_name = std::move(name);
}

inline const std::string& Layout::getName() const{
	return m_name;
}



template<typename T>
inline std::vector<std::reference_wrapper<Layout::PadProxy<T>>> Layout::getPads() {
	const auto pads = findPads<T>();
	std::vector<std::reference_wrapper<PadProxy<T>>> result;
	result.reserve(pads.size());

	std::transform(
		pads.cbegin(), pads.cend(),
		std::back_inserter(result),
		[] (T& pad) -> PadProxy<T>& {
			return makeProxy(pad);
		}
	);
	
	return result;
}

template<typename T>
inline std::vector<std::reference_wrapper<const Layout::PadProxy<T>>> Layout::getPads() const {
	const auto pads = findPads<T>();
	std::vector<std::reference_wrapper<PadProxy<T>>> result;
	result.reserve(pads.size());

	std::transform(
		pads.cbegin(), pads.cend(),
		std::back_inserter(result),
		[] (const T& pad) -> const PadProxy<T>& {
			return makeProxy(pad);
		}
	);
	
	return result;
}

template<typename T>
inline Layout::PadProxy<T>& Layout::getPad(std::string_view name) {
	return makeProxy(findPad<T>(name));
}

template<typename T>
inline Layout::PadProxy<T>& Layout::getPad(const T& pad) {
	return makeProxy(findPad(pad));
}

template<typename T>
inline const Layout::PadProxy<T>& Layout::getPad(std::string_view name) const {
	return makeProxy(findPad<T>(name));
}

template<typename T>
inline const Layout::PadProxy<T>& Layout::getPad(const T& pad) const {
	return makeProxy(findPad(pad));
}



template<typename T>
inline Layout::PadProxy<T>& Layout::makeProxy(T& pad) {
	return static_cast<PadProxy<T>&>(pad);
}

template<typename T>
inline const Layout::PadProxy<T>& Layout::makeProxy(const T& pad) {
	return static_cast<const PadProxy<T>&>(pad);
}



inline void Layout::registerPad(PadRef pad) {
	m_pads.push_back(pad);
}

template<typename T>
inline void Layout::registerPad(PadProxy<T>& pad) {
	registerPad(static_cast<T&>(pad));
}

inline void Layout::registerPads(std::initializer_list<PadRef> pads) {
	m_pads.insert(m_pads.cend(), pads);
}

inline void Layout::removePad(PadRef pad) {
	const auto ite = std::find_if(
		m_pads.cbegin(), m_pads.cend(),
		[pad] (PadRef p) -> bool {
			return &(p.get()) == &(pad.get()); //Compare pointers
		}
	);
	m_pads.erase(ite);
}



template <typename T>
inline T& Layout::findPad(std::string_view str) const {
	static_assert(std::is_base_of<PadBase, T>::value, "T must inherit from PadBase");
	for(const auto& pad : m_pads) {
		if(pad.get().getName() == str){
			auto* p = dynamic_cast<T*>(&(pad.get()));
			if(p) return *p;
		}
	}

	throw Exception("Requested pad not found");
}

template <typename T>
T& Layout::findPad(const T& pad) const {
	return findPad<T>(pad.getName());
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