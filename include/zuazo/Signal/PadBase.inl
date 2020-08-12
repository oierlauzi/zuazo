#include "PadBase.h"

namespace Zuazo::Signal {

inline PadBase::PadBase(std::string name)
	: m_name(std::move(name))
{
}

inline bool PadBase::operator==(const PadBase& other) const {
	return m_name == other.m_name;
}

inline bool PadBase::operator!=(const PadBase& other) const {
	return m_name != other.m_name;
}

inline bool PadBase::operator<(const PadBase& other) const {
	return m_name < other.m_name;
}

inline bool PadBase::operator<=(const PadBase& other) const {
	return m_name <= other.m_name;
}

inline bool PadBase::operator>(const PadBase& other) const {
	return m_name > other.m_name;
}

inline bool PadBase::operator>=(const PadBase& other) const {
	return m_name >= other.m_name;
}


inline void PadBase::setName(std::string name) {
	m_name = std::move(name);
}

inline const std::string& PadBase::getName() const {
	return m_name;
}

}