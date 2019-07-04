#pragma once

#include "PixelComponent.h"
#include "Utils/Macros.h"

#include <array>
#include <utility>
#include <string>

namespace Zuazo {

class PixelFormat {
public:
	static constexpr size_t MAX_PIXEL_COMOPONENTS = 8;
	typedef std::array<PixelComponent, MAX_PIXEL_COMOPONENTS> ComponentArray;

	enum PlanarType {
		NONE			=0, 				///There is 1 plane with 1 component. For istance Y8
		PACKED			=ZUAZO_BIT(0),		///There is only 1 plane with multiple components. For istance RGB24
		PLANAR			=ZUAZO_BIT(1),		///There are multiple planes with 1 component each. For istance YUV444_32
		SEMI_PLANAR		=PACKED | PLANAR,	///There are multiple planes and at least one of them has multiple components. For istance NV12
	};

	constexpr PixelFormat() = default;
	constexpr PixelFormat(const std::initializer_list<PixelComponent>& comp);
	constexpr PixelFormat(const PixelFormat& other) = default;
	~PixelFormat() = default;

	constexpr operator bool() const;

	constexpr const ComponentArray&		getComponents() const;

	constexpr uint              		getComponentCount() const;
	constexpr uint 						getLength() const;
	constexpr uint              		getPlaneCount() const;
	constexpr Math::Rational32_t		getSize() const;
	constexpr Math::Rational32_t		getPlaneSize(uint plane) const;
	constexpr PlanarType				getPlanarType() const;
	constexpr uint						getOffset(ComponentArray::const_iterator el) const;

	constexpr bool                      hasColor() const;
	constexpr bool                      hasAlpha() const;
private:
	ComponentArray						m_components;
};

} // namespace Zuazo

#include "PixelFormat.inl"

namespace Zuazo::PixelFormats {
	constexpr PixelFormat fourcc(uint32_t fcc);
	
	#define ZUAZO_FOURCC(x) \
		(ZUAZO_TO_STRING(x)[0] << 3 | ZUAZO_TO_STRING(x)[1] << 2 | ZUAZO_TO_STRING(x)[2] << 1 | ZUAZO_TO_STRING(x)[3])
	

	constexpr PixelFormat NONE;

	/*
	 * PACKED FORMATS
	 */

	constexpr PixelFormat Y8		{ PixelComponents::Y8 };
	constexpr PixelFormat Y16		{ PixelComponents::Y16 };
	constexpr PixelFormat Y16f		{ PixelComponents::Y16f };
	constexpr PixelFormat Y32		{ PixelComponents::Y32 };
	constexpr PixelFormat Y32f		{ PixelComponents::Y32f };

	constexpr PixelFormat YA16		{ PixelComponents::Y8, PixelComponents::A8 };
	constexpr PixelFormat YA32		{ PixelComponents::Y16, PixelComponents::A16 };
	constexpr PixelFormat YA32f		{ PixelComponents::Y16f, PixelComponents::A16f };
	constexpr PixelFormat YA64		{ PixelComponents::Y32, PixelComponents::A32 };
	constexpr PixelFormat YA64f		{ PixelComponents::Y32f, PixelComponents::A32f };

	constexpr PixelFormat RGB24     { PixelComponents::R8, PixelComponents::G8, PixelComponents::B8 };
	constexpr PixelFormat BGR24     { PixelComponents::B8, PixelComponents::G8, PixelComponents::R8 };
	constexpr PixelFormat RGB48     { PixelComponents::R16, PixelComponents::G16, PixelComponents::B16 };
	constexpr PixelFormat BGR48     { PixelComponents::B16, PixelComponents::G16, PixelComponents::R16 };
	constexpr PixelFormat RGB96     { PixelComponents::R32, PixelComponents::G32, PixelComponents::B32 };
	constexpr PixelFormat BGR96     { PixelComponents::B32, PixelComponents::G32, PixelComponents::R32 };
	constexpr PixelFormat RGB96f    { PixelComponents::R32f, PixelComponents::G32f, PixelComponents::B32f };
	constexpr PixelFormat BGR96f    { PixelComponents::B32f, PixelComponents::G32f, PixelComponents::R32f };

	constexpr PixelFormat XRGB32    { PixelComponents::NONE8, PixelComponents::R8, PixelComponents::G8, PixelComponents::B8 };
	constexpr PixelFormat XBGR32    { PixelComponents::NONE8, PixelComponents::B8, PixelComponents::G8, PixelComponents::R8 };
	constexpr PixelFormat RGBX32    { PixelComponents::R8, PixelComponents::G8, PixelComponents::B8, PixelComponents::NONE8 };
	constexpr PixelFormat BGRX32    { PixelComponents::B8, PixelComponents::G8, PixelComponents::R8, PixelComponents::NONE8 };
	constexpr PixelFormat XRGB64    { PixelComponents::NONE16, PixelComponents::R16, PixelComponents::G16, PixelComponents::B16 };
	constexpr PixelFormat XBGR64    { PixelComponents::NONE16, PixelComponents::B16, PixelComponents::G16, PixelComponents::R16 };
	constexpr PixelFormat RGBX64    { PixelComponents::R16, PixelComponents::G16, PixelComponents::B16, PixelComponents::NONE16 };
	constexpr PixelFormat BGRX64    { PixelComponents::B16, PixelComponents::G16, PixelComponents::R16, PixelComponents::NONE16 };
	constexpr PixelFormat XRGB128   { PixelComponents::NONE32, PixelComponents::R32, PixelComponents::G32, PixelComponents::B32 };
	constexpr PixelFormat XBGR128   { PixelComponents::NONE32, PixelComponents::B32, PixelComponents::G32, PixelComponents::B32 };
	constexpr PixelFormat RGBX128   { PixelComponents::R32, PixelComponents::G32, PixelComponents::B32, PixelComponents::NONE32 };
	constexpr PixelFormat BGRX128   { PixelComponents::B32, PixelComponents::G32, PixelComponents::R32, PixelComponents::NONE32 };
	constexpr PixelFormat XRGB128f  { PixelComponents::NONE32, PixelComponents::R32f, PixelComponents::G32f, PixelComponents::B32f };
	constexpr PixelFormat XBGR128f  { PixelComponents::NONE32, PixelComponents::B32f, PixelComponents::G32f, PixelComponents::B32f };
	constexpr PixelFormat RGBX128f  { PixelComponents::R32f, PixelComponents::G32f, PixelComponents::B32f, PixelComponents::NONE32 };
	constexpr PixelFormat BGRX128f  { PixelComponents::B32f, PixelComponents::G32f, PixelComponents::R32f, PixelComponents::NONE32 };

	constexpr PixelFormat ARGB32    { PixelComponents::A8, PixelComponents::R8, PixelComponents::G8, PixelComponents::B8 };
	constexpr PixelFormat ABGR32    { PixelComponents::A8, PixelComponents::B8, PixelComponents::G8, PixelComponents::R8 };
	constexpr PixelFormat RGBA32    { PixelComponents::R8, PixelComponents::G8, PixelComponents::B8, PixelComponents::A8 };
	constexpr PixelFormat BGRA32    { PixelComponents::B8, PixelComponents::G8, PixelComponents::R8, PixelComponents::A8 };
	constexpr PixelFormat ARGB64    { PixelComponents::A16, PixelComponents::R16, PixelComponents::G16, PixelComponents::B16 };
	constexpr PixelFormat ABGR64    { PixelComponents::A16, PixelComponents::B16, PixelComponents::G16, PixelComponents::R16 };
	constexpr PixelFormat RGBA64    { PixelComponents::R16, PixelComponents::G16, PixelComponents::B16, PixelComponents::A16 };
	constexpr PixelFormat BGRA64    { PixelComponents::B16, PixelComponents::G16, PixelComponents::R16, PixelComponents::A16 };
	constexpr PixelFormat ARGB128   { PixelComponents::A32, PixelComponents::R32, PixelComponents::G32, PixelComponents::B32 };
	constexpr PixelFormat ABGR128   { PixelComponents::A32, PixelComponents::B32, PixelComponents::G32, PixelComponents::R32 };
	constexpr PixelFormat RGBA128   { PixelComponents::R32, PixelComponents::G32, PixelComponents::B32, PixelComponents::A32 };
	constexpr PixelFormat BGRA128   { PixelComponents::B32, PixelComponents::G32, PixelComponents::R32, PixelComponents::A32 };
	constexpr PixelFormat ARGB128f  { PixelComponents::A32f, PixelComponents::R32f, PixelComponents::G32f, PixelComponents::B32f };
	constexpr PixelFormat ABGR128f  { PixelComponents::A32f, PixelComponents::B32f, PixelComponents::G32f, PixelComponents::R32f };
	constexpr PixelFormat RGBA128f  { PixelComponents::R32f, PixelComponents::G32f, PixelComponents::B32f, PixelComponents::A32f };
	constexpr PixelFormat BGRA128f  { PixelComponents::B32f, PixelComponents::G32f, PixelComponents::R32f, PixelComponents::A32f };

	constexpr PixelFormat A2RGB30   { PixelComponents::A2, PixelComponents::R10, PixelComponents::G10, PixelComponents::B10 };
	constexpr PixelFormat A2BGR30   { PixelComponents::A2, PixelComponents::B10, PixelComponents::G10, PixelComponents::R10 };
	constexpr PixelFormat RGB30A2   { PixelComponents::R10, PixelComponents::G10, PixelComponents::B10, PixelComponents::A2 };
	constexpr PixelFormat BGR30A2   { PixelComponents::B10, PixelComponents::G10, PixelComponents::R10, PixelComponents::A2 };

	constexpr PixelFormat AYUV32	{ PixelComponents::A8, PixelComponents::Y8, PixelComponents::U8, PixelComponents::V8 };
	constexpr PixelFormat YUVA32	{ PixelComponents::Y8, PixelComponents::U8, PixelComponents::V8, PixelComponents::A8 };
	constexpr PixelFormat AYVU32	{ PixelComponents::A8, PixelComponents::Y8, PixelComponents::V8, PixelComponents::U8 };
	constexpr PixelFormat YVUA32	{ PixelComponents::Y8, PixelComponents::V8, PixelComponents::U8, PixelComponents::A8 };

	/*
	 * PLANAR FORMATS
	 */

	//												|	Component	|	Plane	|		Subsampling			|
	constexpr PixelFormat YUV444_24	{ PixelComponent(PixelComponents::Y8, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U8, 1, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::V8, 2, PixelComponent::Subsampling(1, 1)) };
	constexpr PixelFormat YUV444_30	{ PixelComponent(PixelComponents::Y10, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U10, 1, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::V10, 2, PixelComponent::Subsampling(1, 1)) };
	constexpr PixelFormat YUV444_36	{ PixelComponent(PixelComponents::Y12, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U12, 1, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::V12, 2, PixelComponent::Subsampling(1, 1)) };
	constexpr PixelFormat YUV444_48	{ PixelComponent(PixelComponents::Y16, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U16, 1, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::V16, 2, PixelComponent::Subsampling(1, 1)) };
	constexpr PixelFormat YUV444 = YUV444_24;

	constexpr PixelFormat YUV440_16	{ PixelComponent(PixelComponents::Y8, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U8, 1, PixelComponent::Subsampling(1, 2)),
									  PixelComponent(PixelComponents::V8, 2, PixelComponent::Subsampling(1, 2)) };
	constexpr PixelFormat YUV440_20	{ PixelComponent(PixelComponents::Y10, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U10, 1, PixelComponent::Subsampling(1, 2)),
									  PixelComponent(PixelComponents::V10, 2, PixelComponent::Subsampling(1, 2)) };
	constexpr PixelFormat YUV440_24	{ PixelComponent(PixelComponents::Y12, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U12, 1, PixelComponent::Subsampling(1, 2)),
									  PixelComponent(PixelComponents::V12, 2, PixelComponent::Subsampling(1, 2)) };
	constexpr PixelFormat YUV440_32	{ PixelComponent(PixelComponents::Y16, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U16, 1, PixelComponent::Subsampling(1, 2)),
									  PixelComponent(PixelComponents::V16, 2, PixelComponent::Subsampling(1, 2)) };
	constexpr PixelFormat YUV440 = YUV440_16;

	constexpr PixelFormat YUV422_16	{ PixelComponent(PixelComponents::Y8, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U8, 1, PixelComponent::Subsampling(2, 1)),
									  PixelComponent(PixelComponents::V8, 2, PixelComponent::Subsampling(2, 1)) };
	constexpr PixelFormat YUV422_20	{ PixelComponent(PixelComponents::Y10, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U10, 1, PixelComponent::Subsampling(2, 1)),
									  PixelComponent(PixelComponents::V10, 2, PixelComponent::Subsampling(2, 1)) };
	constexpr PixelFormat YUV422_24	{ PixelComponent(PixelComponents::Y12, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U12, 1, PixelComponent::Subsampling(2, 1)),
									  PixelComponent(PixelComponents::V12, 2, PixelComponent::Subsampling(2, 1)) };
	constexpr PixelFormat YUV422_32	{ PixelComponent(PixelComponents::Y16, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U16, 1, PixelComponent::Subsampling(2, 1)),
									  PixelComponent(PixelComponents::V16, 2, PixelComponent::Subsampling(2, 1)) };
	constexpr PixelFormat YUV422 = YUV422_16;

	constexpr PixelFormat YUV420_12	{ PixelComponent(PixelComponents::Y8, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U8, 1, PixelComponent::Subsampling(2, 2)),
									  PixelComponent(PixelComponents::V8, 2, PixelComponent::Subsampling(2, 2)) };
	constexpr PixelFormat YUV420_15	{ PixelComponent(PixelComponents::Y10, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U10, 1, PixelComponent::Subsampling(2, 2)),
									  PixelComponent(PixelComponents::V10, 2, PixelComponent::Subsampling(2, 2)) };
	constexpr PixelFormat YUV420_18	{ PixelComponent(PixelComponents::Y12, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U12, 1, PixelComponent::Subsampling(2, 2)),
									  PixelComponent(PixelComponents::V12, 2, PixelComponent::Subsampling(2, 2)) };
	constexpr PixelFormat YUV420_24	{ PixelComponent(PixelComponents::Y16, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U16, 1, PixelComponent::Subsampling(2, 2)),
									  PixelComponent(PixelComponents::V16, 2, PixelComponent::Subsampling(2, 2)) };
	constexpr PixelFormat YUV420 = YUV422_12;

	constexpr PixelFormat YUV411_12	{ PixelComponent(PixelComponents::Y8, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U8, 1, PixelComponent::Subsampling(4, 1)),
									  PixelComponent(PixelComponents::V8, 2, PixelComponent::Subsampling(4, 1)) };
	constexpr PixelFormat YUV411_15	{ PixelComponent(PixelComponents::Y10, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U10, 1, PixelComponent::Subsampling(4, 1)),
									  PixelComponent(PixelComponents::V10, 2, PixelComponent::Subsampling(4, 1)) };
	constexpr PixelFormat YUV411_18	{ PixelComponent(PixelComponents::Y12, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U12, 1, PixelComponent::Subsampling(4, 1)),
									  PixelComponent(PixelComponents::V12, 2, PixelComponent::Subsampling(4, 1)) };
	constexpr PixelFormat YUV411_24	{ PixelComponent(PixelComponents::Y16, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U16, 1, PixelComponent::Subsampling(4, 1)),
									  PixelComponent(PixelComponents::V16, 2, PixelComponent::Subsampling(4, 1)) };
	constexpr PixelFormat YUV411 = YUV411_12;

	constexpr PixelFormat YUV410_10	{ PixelComponent(PixelComponents::Y8, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U8, 1, PixelComponent::Subsampling(4, 2)),
									  PixelComponent(PixelComponents::V8, 2, PixelComponent::Subsampling(4, 2)) };
	constexpr PixelFormat YUV410_10_5{PixelComponent(PixelComponents::Y10, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U10, 1, PixelComponent::Subsampling(4, 2)),
									  PixelComponent(PixelComponents::V10, 2, PixelComponent::Subsampling(4, 2)) };
	constexpr PixelFormat YUV410_15	{ PixelComponent(PixelComponents::Y12, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U12, 1, PixelComponent::Subsampling(4, 2)),
									  PixelComponent(PixelComponents::V12, 2, PixelComponent::Subsampling(4, 2)) };
	constexpr PixelFormat YUV410_20	{ PixelComponent(PixelComponents::Y16, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U16, 1, PixelComponent::Subsampling(4, 2)),
									  PixelComponent(PixelComponents::V16, 2, PixelComponent::Subsampling(4, 2)) };
	constexpr PixelFormat YUV410 = YUV410_10;

	constexpr PixelFormat NV12		{ PixelComponent(PixelComponents::Y8, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::U8, 1, PixelComponent::Subsampling(2, 2)),
									  PixelComponent(PixelComponents::V8, 1, PixelComponent::Subsampling(2, 2)) };

	constexpr PixelFormat NV21		{ PixelComponent(PixelComponents::Y8, 0, PixelComponent::Subsampling(1, 1)),
									  PixelComponent(PixelComponents::V8, 1, PixelComponent::Subsampling(2, 2)),
									  PixelComponent(PixelComponents::U8, 1, PixelComponent::Subsampling(2, 2)) };
}
