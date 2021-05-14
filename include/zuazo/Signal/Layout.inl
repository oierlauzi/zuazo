#include "Layout.h"

#include "PadBase.h"

#include <type_traits>
#include <cassert>
#include <algorithm>
#include <tuple>

namespace Zuazo::Signal {

/*
 * Layout
 */

inline Layout::Layout(std::string name)
	: m_name(std::move(name))
{
}

template<typename InputIt>
inline Layout::Layout(std::string name, InputIt beginPads, InputIt endPads)
	: m_name(std::move(name))
	, m_pads(beginPads, endPads)
{
}



inline void Layout::setName(std::string name) noexcept {
	m_name = std::move(name);
}

inline const std::string& Layout::getName() const noexcept {
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
inline Layout::PadProxy<T>* Layout::getPad(std::string_view name) noexcept {
	return makeProxy(findPad<T>(name));
}

template<typename T>
inline const Layout::PadProxy<T>* Layout::getPad(std::string_view name) const noexcept {
	return makeProxy(findPad<T>(name));
}



template<typename T>
inline Layout::PadProxy<T>& Layout::makeProxy(T& pad) noexcept {
	return static_cast<PadProxy<T>&>(pad);
}

template<typename T>
inline Layout::PadProxy<T>* Layout::makeProxy(T* pad) noexcept {
	return static_cast<PadProxy<T>*>(pad);
}

template<typename T>
inline const Layout::PadProxy<T>& Layout::makeProxy(const T& pad) noexcept {
	return static_cast<const PadProxy<T>&>(pad);
}

template<typename T>
inline const Layout::PadProxy<T>* Layout::makeProxy(const T* pad) noexcept {
	return static_cast<const PadProxy<T>*>(pad);
}


inline void Layout::registerPad(PadRef pad) {
	m_pads.push_back(pad);
}

template<typename T>
inline void Layout::registerPad(PadProxy<T>& pad) {
	registerPad(static_cast<T&>(pad));
}

template<typename InputIt>
inline void Layout::registerPad(InputIt begin, InputIt end) {
	m_pads.insert(m_pads.cend(), begin, end);
}

inline void Layout::removePad(PadRef pad) noexcept {
	const auto ite = std::find_if(
		m_pads.cbegin(), m_pads.cend(),
		[pad] (PadRef p) -> bool {
			return &(p.get()) == &(pad.get()); //Compare pointers
		}
	);
	m_pads.erase(ite);
}

template<typename T>
inline void Layout::removePad(PadProxy<T>& pad) {
	removePad(static_cast<PadBase&>(pad));
}


template <typename T>
inline T* Layout::findPad(std::string_view str) const noexcept {
	T* result = nullptr;

	for(PadBase& pad : m_pads) {
		if(pad.getName() == str) {
			//Name match. Check if type also matches
			result = dynamic_cast<T*>(&pad);
			if(result) {
				//Found!
				break;
			}
		}
	}

	return result;
}

template <typename T>
inline std::vector<std::reference_wrapper<T>> Layout::findPads() const {
	static_assert(std::is_base_of<PadBase, T>::value, "T must inherit from PadBase");
	std::vector<T> result;

	for(PadBase& pad : m_pads) {
		auto* ptr = dynamic_cast<T*>(&pad);
		if(ptr) {
			result.emplace_back(*ptr);
		}
	}

	return result;
}

}