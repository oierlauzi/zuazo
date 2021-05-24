#include "PadBase.h"

namespace Zuazo::Signal {

inline PadBase::PadBase(const Layout& layout, std::string name) noexcept
	: m_layout(layout) 
	, m_name(std::move(name))
{
}

inline bool PadBase::operator==(const PadBase& other) const noexcept {
	return m_name == other.m_name;
}

inline bool PadBase::operator!=(const PadBase& other) const noexcept {
	return m_name != other.m_name;
}

inline bool PadBase::operator<(const PadBase& other) const noexcept {
	return m_name < other.m_name;
}

inline bool PadBase::operator<=(const PadBase& other) const noexcept {
	return m_name <= other.m_name;
}

inline bool PadBase::operator>(const PadBase& other) const noexcept {
	return m_name > other.m_name;
}

inline bool PadBase::operator>=(const PadBase& other) const noexcept {
	return m_name >= other.m_name;
}



inline PadProxy<PadBase>& PadBase::getProxy() noexcept {
	return static_cast<PadProxy<PadBase>&>(*this);
}

inline const PadProxy<PadBase>& PadBase::getProxy() const noexcept {
	return static_cast<const PadProxy<PadBase>&>(*this);
}


inline void PadBase::setLayout(const Layout& layout) noexcept {
	m_layout = layout;
}

inline const Layout& PadBase::getLayout() const noexcept {
	return m_layout;
}


inline void PadBase::setName(std::string name) noexcept {
	m_name = std::move(name);
}

inline const std::string& PadBase::getName() const noexcept {
	return m_name;
}

}