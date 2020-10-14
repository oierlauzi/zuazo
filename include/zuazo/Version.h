#pragma once

#include "Utils/CPU.h"

#include <cstdint>
#include <cstddef>
#include <ostream>

namespace Zuazo {

class Version {
public:
	constexpr Version(uint32_t major = 0, uint32_t minor = 0, uint32_t patch = 0) noexcept;
	constexpr Version(const Version& other) noexcept = default;
	~Version() = default;

	constexpr Version& operator=(const Version& other) noexcept = default;

	constexpr bool operator==(const Version& other) const noexcept;
	constexpr bool operator!=(const Version& other) const noexcept;
	constexpr bool operator<(const Version& other) const noexcept;
	constexpr bool operator<=(const Version& other) const noexcept;
	constexpr bool operator>(const Version& other) const noexcept;
	constexpr bool operator>=(const Version& other) const noexcept;

	constexpr uint32_t getMajor() const noexcept;
	constexpr uint32_t getMinor() const noexcept;
	constexpr uint32_t getPatch() const noexcept;
private:
	using DataType = uint32_t;

	DataType m_data;

	static constexpr size_t DATA_BITS = sizeof(DataType) * Utils::getByteSize();

	static constexpr size_t MAJOR_BITS = 16;
	static constexpr size_t MINOR_BITS = 8;
	static constexpr size_t PATCH_BITS = 8;

	static_assert((MAJOR_BITS + MINOR_BITS + PATCH_BITS) == DATA_BITS);

	static constexpr size_t MAJOR_OFF = PATCH_BITS + MINOR_BITS;
	static constexpr size_t MINOR_OFF = PATCH_BITS;
	static constexpr size_t PATCH_OFF = 0U;	

	static constexpr DataType MAJOR_MASK = static_cast<DataType>(-1) >> (DATA_BITS - MAJOR_BITS) << MAJOR_OFF;
	static constexpr DataType MINOR_MASK = static_cast<DataType>(-1) >> (DATA_BITS - MINOR_BITS) << MINOR_OFF;
	static constexpr DataType PATCH_MASK = static_cast<DataType>(-1) >> (DATA_BITS - PATCH_BITS) << PATCH_OFF;
};

std::ostream& operator<<(std::ostream& os, Version ver);

}

#include "Version.inl"