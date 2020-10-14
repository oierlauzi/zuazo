#include "Version.h"

#include "Utils/Functions.h"

namespace Zuazo {

constexpr Version::Version(uint32_t major, uint32_t minor, uint32_t patch) noexcept
	: m_data(	((major << MAJOR_OFF) & MAJOR_MASK) |
				((minor << MINOR_OFF) & MINOR_MASK) |
				((patch << PATCH_OFF) & PATCH_MASK) )
{
}

constexpr bool Version::operator==(const Version& other) const noexcept {
	return m_data == other.m_data;
}

constexpr bool Version::operator!=(const Version& other) const noexcept {
	return m_data != other.m_data;
}

constexpr bool Version::operator<(const Version& other) const noexcept {
	return m_data < other.m_data;
}

constexpr bool Version::operator<=(const Version& other) const noexcept {
	return m_data <= other.m_data;
}

constexpr bool Version::operator>(const Version& other) const noexcept {
	return m_data > other.m_data;
}

constexpr bool Version::operator>=(const Version& other) const noexcept {
	return m_data >= other.m_data;
}


constexpr uint32_t Version::getMajor() const noexcept {
	return ((m_data & MAJOR_MASK) >> MAJOR_OFF);
}

constexpr uint32_t Version::getMinor() const noexcept {
	return ((m_data & MINOR_MASK) >> MINOR_OFF);
}

constexpr uint32_t Version::getPatch() const noexcept {
	return ((m_data & PATCH_MASK) >> PATCH_OFF);
}



inline std::ostream& operator<<(std::ostream& os, Version ver) {
	return os << ver.getMajor() << '.' << ver.getMinor() << '.' << ver.getPatch();
}

}