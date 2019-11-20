#pragma once

#include "PixelComponent.h"
#include "../Macros.h"

#include <array>
#include <utility>
#include <string>

namespace Zuazo::Graphics {

struct PixelFormat{
	static constexpr size_t MAX_COMPONENTS = 4;

	std::array<PixelComponent, MAX_COMPONENTS> components;

	constexpr PixelFormat() = default;
	constexpr PixelFormat(const PixelFormat& other) = default;
	~PixelFormat() = default;

	PixelFormat&			operator=(const PixelFormat& other) = default;

	constexpr 				operator bool() const;
	constexpr bool			operator==(const PixelFormat& other) const;
	constexpr bool			operator!=(const PixelFormat& other) const;


	constexpr bool			isRGB() const;
	constexpr bool			isYUV() const;
	constexpr bool			isPacked() const;
	constexpr bool			isPlanar() const;
	constexpr bool			hasColor() const;
	constexpr bool			hasAlpha() const;
	constexpr size_t		getComponentCount() const;
	constexpr size_t		getPlaneCount() const;
};

} // namespace Zuazo

#include "PixelFormat.inl"

namespace Zuazo::Graphics::PixelFormats {
	
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

	//TODO add packed YUV formats

	/*
	 * PLANAR FORMATS
	 */

	constexpr PixelFormat YUV444_24	{ PixelComponents::Y8,
									  modifySubsampling(modifyPlane(PixelComponents::U8,	1),	{1, 1}),
									  modifySubsampling(modifyPlane(PixelComponents::V8, 	2),	{1, 1}), };
	constexpr PixelFormat YUV444_30	{ PixelComponents::Y10,
									  modifySubsampling(modifyPlane(PixelComponents::U10, 	1),	{1, 1}),
									  modifySubsampling(modifyPlane(PixelComponents::V10, 	2),	{1, 1}), };
	constexpr PixelFormat YUV444_36	{ PixelComponents::Y12,
									  modifySubsampling(modifyPlane(PixelComponents::U12, 	1),	{1, 1}),
									  modifySubsampling(modifyPlane(PixelComponents::V12, 	2),	{1, 1}), };
	constexpr PixelFormat YUV444_48	{ PixelComponents::Y16,
									  modifySubsampling(modifyPlane(PixelComponents::U16, 	1),	{1, 1}),
									  modifySubsampling(modifyPlane(PixelComponents::V16, 	2),	{1, 1}), };
	constexpr PixelFormat YUV444 = YUV444_24;



	constexpr PixelFormat YUV422_16	{ PixelComponents::Y8,
									  modifySubsampling(modifyPlane(PixelComponents::U8,	1),	{2, 1}),
									  modifySubsampling(modifyPlane(PixelComponents::V8, 	2),	{2, 1}), };
	constexpr PixelFormat YUV422_20	{ PixelComponents::Y10,
									  modifySubsampling(modifyPlane(PixelComponents::U10, 	1),	{2, 1}),
									  modifySubsampling(modifyPlane(PixelComponents::V10, 	2),	{2, 1}), };
	constexpr PixelFormat YUV422_24	{ PixelComponents::Y12,
									  modifySubsampling(modifyPlane(PixelComponents::U12, 	1),	{2, 1}),
									  modifySubsampling(modifyPlane(PixelComponents::V12, 	2),	{2, 1}), };
	constexpr PixelFormat YUV422_32	{ PixelComponents::Y16,
									  modifySubsampling(modifyPlane(PixelComponents::U16, 	1),	{2, 1}),
									  modifySubsampling(modifyPlane(PixelComponents::V16, 	2),	{2, 1}), };
	constexpr PixelFormat YUV422 = YUV422_16;



	constexpr PixelFormat YUV420_12	{ PixelComponents::Y8,
									  modifySubsampling(modifyPlane(PixelComponents::U8,	1),	{2, 2}),
									  modifySubsampling(modifyPlane(PixelComponents::V8, 	2),	{2, 2}), };
	constexpr PixelFormat YUV420_15	{ PixelComponents::Y10,
									  modifySubsampling(modifyPlane(PixelComponents::U10, 	1),	{2, 2}),
									  modifySubsampling(modifyPlane(PixelComponents::V10, 	2),	{2, 2}), };
	constexpr PixelFormat YUV420_18	{ PixelComponents::Y12,
									  modifySubsampling(modifyPlane(PixelComponents::U12, 	1),	{2, 2}),
									  modifySubsampling(modifyPlane(PixelComponents::V12, 	2),	{2, 2}), };
	constexpr PixelFormat YUV420_24	{ PixelComponents::Y16,
									  modifySubsampling(modifyPlane(PixelComponents::U16, 	1),	{2, 2}),
									  modifySubsampling(modifyPlane(PixelComponents::V16, 	2),	{2, 2}), };
	constexpr PixelFormat YUV420 = YUV420_12;



	constexpr PixelFormat YUV411_12	{ PixelComponents::Y8,
									  modifySubsampling(modifyPlane(PixelComponents::U8,	1),	{4, 1}),
									  modifySubsampling(modifyPlane(PixelComponents::V8, 	2),	{4, 1}), };
	constexpr PixelFormat YUV411_15	{ PixelComponents::Y10,
									  modifySubsampling(modifyPlane(PixelComponents::U10, 	1),	{4, 1}),
									  modifySubsampling(modifyPlane(PixelComponents::V10, 	2),	{4, 1}), };
	constexpr PixelFormat YUV411_18	{ PixelComponents::Y12,
									  modifySubsampling(modifyPlane(PixelComponents::U12, 	1),	{4, 1}),
									  modifySubsampling(modifyPlane(PixelComponents::V12, 	2),	{4, 1}), };
	constexpr PixelFormat YUV411_24	{ PixelComponents::Y16,
									  modifySubsampling(modifyPlane(PixelComponents::U16, 	1),	{4, 1}),
									  modifySubsampling(modifyPlane(PixelComponents::V16, 	2),	{4, 1}), };
	constexpr PixelFormat YUV411 = YUV411_12;



	constexpr PixelFormat YUV410_10	{ PixelComponents::Y8,
									  modifySubsampling(modifyPlane(PixelComponents::U8,	1),	{4, 2}),
									  modifySubsampling(modifyPlane(PixelComponents::V8, 	2),	{4, 2}), };
	constexpr PixelFormat YUV410_10_5{ PixelComponents::Y10,
									  modifySubsampling(modifyPlane(PixelComponents::U10, 	1),	{4, 2}),
									  modifySubsampling(modifyPlane(PixelComponents::V10, 	2),	{4, 2}), };
	constexpr PixelFormat YUV410_15	{ PixelComponents::Y12,
									  modifySubsampling(modifyPlane(PixelComponents::U12, 	1),	{4, 2}),
									  modifySubsampling(modifyPlane(PixelComponents::V12, 	2),	{4, 2}), };
	constexpr PixelFormat YUV410_20	{ PixelComponents::Y16,
									  modifySubsampling(modifyPlane(PixelComponents::U16, 	1),	{4, 2}),
									  modifySubsampling(modifyPlane(PixelComponents::V16, 	2),	{4, 2}), };
	constexpr PixelFormat YUV410 = YUV410_10;



	constexpr PixelFormat NV12		{ PixelComponents::Y8,
									  modifySubsampling(modifyPlane(PixelComponents::U8,	1),	{2, 2}),
									  modifySubsampling(modifyPlane(PixelComponents::V8, 	1),	{2, 2}), };
	constexpr PixelFormat NV21		{ PixelComponents::Y10,
									  modifySubsampling(modifyPlane(PixelComponents::U10, 	1),	{2, 2}),
									  modifySubsampling(modifyPlane(PixelComponents::V10, 	1),	{2, 2}), };
}
