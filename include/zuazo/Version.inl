#include "Version.h"

#include "Utils/Functions.h"

#include <charconv>

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

inline bool fromString(std::string_view str, Version& ver) noexcept {
	uint major, minor, patch;
	std::from_chars_result ret;
	ret.ptr = str.cbegin();
	
	//Parse the major version
	ret = std::from_chars(ret.ptr, str.cend(), major);
	if(ret.ec != std::errc()) {
		return false;
	}

	//Expect a point afterwards
	if(ret.ptr != str.cend()) {
		if(*(ret.ptr++) != '.') {
			return false;
		}
	}

	//Parse the minor version
	ret = std::from_chars(ret.ptr, str.cend(), minor);
	if(ret.ec  != std::errc()) {
		return false;
	}

	//Expect a point afterwards
	if(ret.ptr != str.cend()) {
		if(*(ret.ptr++) != '.') {
			return false;
		}
	}

	//Parse the patch
	ret = std::from_chars(ret.ptr, str.cend(), patch);
	if(ret.ec  != std::errc()) {
		return false;
	}

	ver = Version(major, minor, patch);
	return true;
}	

}