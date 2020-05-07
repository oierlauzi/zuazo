#include "Version.h"

#include "Utils/Functions.h"

namespace Zuazo {

constexpr Version::Version(uint32_t major, uint32_t minor, uint32_t patch) :
	m_data(	((major << MAJOR_OFF) & MAJOR_MASK) |
			((minor << MINOR_OFF) & MINOR_MASK) |
			((patch << PATCH_OFF) & PATCH_MASK) )
{
}

constexpr bool Version::operator==(const Version& other) const {
	return m_data == other.m_data;
}

constexpr bool Version::operator!=(const Version& other) const {
	return m_data != other.m_data;
}

constexpr bool Version::operator<(const Version& other) const {
	return m_data < other.m_data;
}

constexpr bool Version::operator<=(const Version& other) const {
	return m_data <= other.m_data;
}

constexpr bool Version::operator>(const Version& other) const {
	return m_data > other.m_data;
}

constexpr bool Version::operator>=(const Version& other) const {
	return m_data >= other.m_data;
}


constexpr uint32_t Version::getMajor() const {
	return ((m_data & MAJOR_MASK) >> MAJOR_OFF);
}

constexpr uint32_t Version::getMinor() const {
	return ((m_data & MINOR_MASK) >> MINOR_OFF);
}

constexpr uint32_t Version::getPatch() const {
	return ((m_data & PATCH_MASK) >> PATCH_OFF);
}



inline std::string toString(Version ver) {
	return 	toString(ver.getMajor()) + "." +
			toString(ver.getMinor()) + "." +
			toString(ver.getPatch());
}

}