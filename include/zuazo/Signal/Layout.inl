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
inline std::vector<std::reference_wrapper<PadProxy<T>>> Layout::getPads() {
	return reinterpret_cast<std::vector<std::reference_wrapper<PadProxy<T>>>&&>(findPads<T>()); //HACK
}

template<typename T>
inline std::vector<std::reference_wrapper<const PadProxy<T>>> Layout::getPads() const {
	return reinterpret_cast<std::vector<std::reference_wrapper<const PadProxy<T>>>&&>(findPads<T>()); //HACK
}

template<typename T>
inline PadProxy<T>* Layout::getPad(std::string_view name) noexcept {
	auto* pad = findPad<T>(name);
	return pad ? &(pad->getProxy()) : nullptr;
}

template<typename T>
inline const PadProxy<T>* Layout::getPad(std::string_view name) const noexcept {
	const auto* pad = findPad<T>(name);
	return pad ? &(pad->getProxy()) : nullptr;
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
	std::vector<std::reference_wrapper<T>> result;

	for(PadBase& pad : m_pads) {
		auto* ptr = dynamic_cast<T*>(&pad);
		if(ptr) {
			result.emplace_back(*ptr);
		}
	}

	return result;
}

}