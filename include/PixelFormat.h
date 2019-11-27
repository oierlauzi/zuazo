#pragma once

#include "PixelComponent.h"
#include "Macros.h"

#include <array>
#include <utility>
#include <string>

namespace Zuazo {

struct PixelFormat{
	static constexpr size_t MAX_COMPONENTS = 4;
	using ComponentArray = std::array<PixelComponent, MAX_COMPONENTS>;

	ComponentArray 	components = {};
	uint32_t		wordSize = 0;

	constexpr PixelFormat() = default;
	constexpr PixelFormat(const PixelFormat& other) = default;
	~PixelFormat() = default;

	PixelFormat&			operator=(const PixelFormat& other) = default;

	constexpr 				operator bool() const;
	constexpr bool			operator==(const PixelFormat& other) const;
	constexpr bool			operator!=(const PixelFormat& other) const;


	constexpr bool			isRGB() const;
	constexpr bool			isYUV() const;
	constexpr bool			isPlanar() const;
	constexpr bool			hasColor() const;
	constexpr bool			hasAlpha() const;
	constexpr size_t		getComponentCount() const;
	constexpr size_t		getPlaneCount() const;
};

constexpr PixelFormat modifyComponents(const PixelFormat& fmt, const PixelFormat::ComponentArray& newComponents);
constexpr PixelFormat modifyWordSize(const PixelFormat& fmt, uint32_t newWordSize);
constexpr PixelFormat getMemoryRepresentation(const PixelFormat& fmt);
constexpr PixelFormat reverse(const PixelFormat& fmt);

} // namespace Zuazo

#include "PixelFormat.inl"

namespace Zuazo::PixelFormats {

	constexpr PixelFormat NONE;

	/*
	 * PACKED FORMATS
	 */

	constexpr PixelFormat Y8				{ { PixelComponents::Y8 } };
	constexpr PixelFormat Y16				{ { PixelComponents::Y16 } };
	constexpr PixelFormat Y32				{ { PixelComponents::Y32 } };
	constexpr PixelFormat Y32f				{ { PixelComponents::Y32f } };
	constexpr PixelFormat Y64				{ { PixelComponents::Y64 } };
	constexpr PixelFormat Y64f				{ { PixelComponents::Y64f } };

	constexpr PixelFormat Y8A8				{ { PixelComponents::Y8, PixelComponents::A8 } };
	constexpr PixelFormat A8Y8				{ { PixelComponents::A8, PixelComponents::Y8 } };
	constexpr PixelFormat Y16A16			{ { PixelComponents::Y16, PixelComponents::A16 } };
	constexpr PixelFormat A16Y16			{ { PixelComponents::A16, PixelComponents::Y16 } };
	constexpr PixelFormat Y32A32			{ { PixelComponents::Y32, PixelComponents::A32 } };
	constexpr PixelFormat A32Y32			{ { PixelComponents::A32, PixelComponents::Y32 } };
	constexpr PixelFormat Y32fA32f			{ { PixelComponents::Y32f, PixelComponents::A32f } };
	constexpr PixelFormat A32fY32f			{ { PixelComponents::A32f, PixelComponents::Y32f } };
	constexpr PixelFormat Y64A64			{ { PixelComponents::Y64, PixelComponents::A64 } };
	constexpr PixelFormat A64Y64			{ { PixelComponents::A64, PixelComponents::Y64 } };
	constexpr PixelFormat Y64fA64f			{ { PixelComponents::Y64f, PixelComponents::A64f } };
	constexpr PixelFormat A64fY64f			{ { PixelComponents::A64f, PixelComponents::Y64f } };

	constexpr PixelFormat R8G8B8			{ { PixelComponents::R8, PixelComponents::G8, PixelComponents::B8 } };
	constexpr PixelFormat B8G8R8			{ { PixelComponents::B8, PixelComponents::G8, PixelComponents::R8 } };
	constexpr PixelFormat R16G16B16			{ { PixelComponents::R16, PixelComponents::G16, PixelComponents::B16 } };
	constexpr PixelFormat B16G16R16			{ { PixelComponents::B16, PixelComponents::G16, PixelComponents::R16 } };
	constexpr PixelFormat R32G32B32			{ { PixelComponents::R32, PixelComponents::G32, PixelComponents::B32 } };
	constexpr PixelFormat B32G32R32 		{ { PixelComponents::B32, PixelComponents::G32, PixelComponents::R32 } };
	constexpr PixelFormat R32fG32fB32f		{ { PixelComponents::R32f, PixelComponents::G32f, PixelComponents::B32f } };
	constexpr PixelFormat B32fG32fR32f		{ { PixelComponents::B32f, PixelComponents::G32f, PixelComponents::R32f } };

	constexpr PixelFormat A8R8G8B8			{ { PixelComponents::A8, PixelComponents::R8, PixelComponents::G8, PixelComponents::B8 } };
	constexpr PixelFormat A8B8G8R8			{ { PixelComponents::A8, PixelComponents::B8, PixelComponents::G8, PixelComponents::R8 } };
	constexpr PixelFormat R8G8B8A8			{ { PixelComponents::R8, PixelComponents::G8, PixelComponents::B8, PixelComponents::A8 } };
	constexpr PixelFormat B8G8R8A8			{ { PixelComponents::B8, PixelComponents::G8, PixelComponents::R8, PixelComponents::A8 } };
	constexpr PixelFormat A16R16G16B16		{ { PixelComponents::A16, PixelComponents::R16, PixelComponents::G16, PixelComponents::B16 } };
	constexpr PixelFormat A16B16G16R16		{ { PixelComponents::A16, PixelComponents::B16, PixelComponents::G16, PixelComponents::R16 } };
	constexpr PixelFormat R16G16B16A16		{ { PixelComponents::R16, PixelComponents::G16, PixelComponents::B16, PixelComponents::A16 } };
	constexpr PixelFormat B16G16R16A16		{ { PixelComponents::B16, PixelComponents::G16, PixelComponents::R16, PixelComponents::A16 } };
	constexpr PixelFormat A32R32G32B32		{ { PixelComponents::A32, PixelComponents::R32, PixelComponents::G32, PixelComponents::B32 } };
	constexpr PixelFormat A32B32G32R32		{ { PixelComponents::A32, PixelComponents::B32, PixelComponents::G32, PixelComponents::R32 } };
	constexpr PixelFormat R32G32B32A32		{ { PixelComponents::R32, PixelComponents::G32, PixelComponents::B32, PixelComponents::A32 } };
	constexpr PixelFormat B32G32R32A32		{ { PixelComponents::B32, PixelComponents::G32, PixelComponents::R32, PixelComponents::A32 } };
	constexpr PixelFormat A32fR32fG32fB32f	{ { PixelComponents::A32f, PixelComponents::R32f, PixelComponents::G32f, PixelComponents::B32f } };
	constexpr PixelFormat A32fB32fG32fR32f	{ { PixelComponents::A32f, PixelComponents::B32f, PixelComponents::G32f, PixelComponents::R32f } };
	constexpr PixelFormat R32fG32fB32fA32f	{ { PixelComponents::R32f, PixelComponents::G32f, PixelComponents::B32f, PixelComponents::A32f } };
	constexpr PixelFormat B32fG32fR32fA32f	{ { PixelComponents::B32f, PixelComponents::G32f, PixelComponents::R32f, PixelComponents::A32f } };


	//CPU-word packed formats
	constexpr PixelFormat Y8A8_16			= modifyWordSize(Y8A8, 16);
	constexpr PixelFormat A8Y8_16			= modifyWordSize(A8Y8, 16);
	constexpr PixelFormat R5G6B5_16			{ { PixelComponents::R5, PixelComponents::G6, PixelComponents::B5 }, 16 };
	constexpr PixelFormat B5G6R5_16			{ { PixelComponents::B5, PixelComponents::G6, PixelComponents::R5 }, 16 };
	constexpr PixelFormat Y16A16_32			= modifyWordSize(Y16A16, 32);
	constexpr PixelFormat A16Y16_32			= modifyWordSize(A16Y16, 32);
	constexpr PixelFormat R10G12B10_32		{ { PixelComponents::R10, PixelComponents::G12, PixelComponents::B10 }, 32 };
	constexpr PixelFormat B10G12R10_32		{ { PixelComponents::B10, PixelComponents::G12, PixelComponents::R10 }, 32 };
	constexpr PixelFormat A8R8G8B8_32		= modifyWordSize(A8R8G8B8, 32);
	constexpr PixelFormat A8B8G8R8_32		= modifyWordSize(A8B8G8R8, 32);
	constexpr PixelFormat R8G8B8A8_32		= modifyWordSize(R8G8B8A8, 32);
	constexpr PixelFormat B8G8R8A8_32		= modifyWordSize(B8G8R8A8, 32);
	constexpr PixelFormat X8R8G8B8_32		{ { PixelComponents::NONE8, PixelComponents::R8, PixelComponents::G8, PixelComponents::B8 }, 32 };
	constexpr PixelFormat X8B8G8R8_32		{ { PixelComponents::NONE8, PixelComponents::B8, PixelComponents::G8, PixelComponents::R8 }, 32 };
	constexpr PixelFormat R8G8B8X8_32		{ { PixelComponents::R8, PixelComponents::G8, PixelComponents::B8, PixelComponents::NONE8 }, 32 };
	constexpr PixelFormat B8G8R8X8_32		{ { PixelComponents::B8, PixelComponents::G8, PixelComponents::R8, PixelComponents::NONE8 }, 32 };
	constexpr PixelFormat A2R10G10B10_32	{ { PixelComponents::A2, PixelComponents::R10, PixelComponents::G10, PixelComponents::B10 }, 32 };
	constexpr PixelFormat A2B10G10R10_32	{ { PixelComponents::A2, PixelComponents::B10, PixelComponents::G10, PixelComponents::R10 }, 32 };
	constexpr PixelFormat R10G10B10A2_32	{ { PixelComponents::R10, PixelComponents::G10, PixelComponents::B10, PixelComponents::A2 }, 32 };
	constexpr PixelFormat B10G10R10A2_32	{ { PixelComponents::B10, PixelComponents::G10, PixelComponents::R10, PixelComponents::A2 }, 32 };
	constexpr PixelFormat Y32A32_64			= modifyWordSize(Y32A32, 64);
	constexpr PixelFormat A32Y32_64			= modifyWordSize(A32Y32, 64);
	constexpr PixelFormat R20G24B20_64		{ { PixelComponents::R20, PixelComponents::G24, PixelComponents::B20 }, 64 };
	constexpr PixelFormat B20G24R20_64		{ { PixelComponents::B20, PixelComponents::G24, PixelComponents::R20 }, 64 };
	constexpr PixelFormat A16R16G16B16_64	= modifyWordSize(A16R16G16B16, 64);
	constexpr PixelFormat A16B16G16R16_64	= modifyWordSize(A16B16G16R16, 64);
	constexpr PixelFormat R16G16B16A16_64	= modifyWordSize(R16G16B16A16, 64);
	constexpr PixelFormat B16G16R16A16_64	= modifyWordSize(B16G16R16A16, 64);
	constexpr PixelFormat X16R16G16B16_64	{ { PixelComponents::NONE16, PixelComponents::R16, PixelComponents::G16, PixelComponents::B16 }, 64 };
	constexpr PixelFormat X16B16G16R16_64	{ { PixelComponents::NONE16, PixelComponents::B16, PixelComponents::G16, PixelComponents::R16 }, 64 };
	constexpr PixelFormat R16G16B16X16_64	{ { PixelComponents::R16, PixelComponents::G16, PixelComponents::B16, PixelComponents::NONE16 }, 64 };
	constexpr PixelFormat B16G16R16X16_64	{ { PixelComponents::B16, PixelComponents::G16, PixelComponents::R16, PixelComponents::NONE16 }, 64 };
	constexpr PixelFormat A4R10G10B10_64	{ { PixelComponents::A4, PixelComponents::R20, PixelComponents::G20, PixelComponents::B20 }, 64 };
	constexpr PixelFormat A4B10G10R10_64	{ { PixelComponents::A4, PixelComponents::B20, PixelComponents::G20, PixelComponents::R20 }, 64 };
	constexpr PixelFormat R10G10B10A4_64	{ { PixelComponents::R20, PixelComponents::G20, PixelComponents::B20, PixelComponents::A4 }, 64 };
	constexpr PixelFormat B10G10R10A4_64	{ { PixelComponents::B20, PixelComponents::G20, PixelComponents::R20, PixelComponents::A4 }, 64 };
	
	//TODO Packed YUV formats

	/*
	 * PLANAR FORMATS
	 */


	constexpr PixelFormat Y8U8V8_444		{ {	PixelComponents::Y8,
									  			modifySubsampling(modifyPlane(PixelComponents::U8,		1),	{1, 1}),
									  			modifySubsampling(modifyPlane(PixelComponents::V8, 		2),	{1, 1}) } };
	constexpr PixelFormat Y10U10V10_444		{ { PixelComponents::Y10,
									  			modifySubsampling(modifyPlane(PixelComponents::U10, 	1),	{1, 1}),
									  			modifySubsampling(modifyPlane(PixelComponents::V10, 	2),	{1, 1}) } };
	constexpr PixelFormat Y12U12V12_444		{ { PixelComponents::Y12,
									  			modifySubsampling(modifyPlane(PixelComponents::U12, 	1),	{1, 1}),
									  			modifySubsampling(modifyPlane(PixelComponents::V12, 	2),	{1, 1}) } };
	constexpr PixelFormat Y16U16V16_444		{ {	PixelComponents::Y16,
									  			modifySubsampling(modifyPlane(PixelComponents::U16, 	1),	{1, 1}),
									  			modifySubsampling(modifyPlane(PixelComponents::V16, 	2),	{1, 1}) } };


	constexpr PixelFormat Y8U8V8_422		{ {	PixelComponents::Y8,
									  			modifySubsampling(modifyPlane(PixelComponents::U8,		1),	{2, 1}),
									  			modifySubsampling(modifyPlane(PixelComponents::V8, 		2),	{2, 1}) } };
	constexpr PixelFormat Y10U10V10_422		{ { PixelComponents::Y10,
									  			modifySubsampling(modifyPlane(PixelComponents::U10, 	1),	{2, 1}),
									  			modifySubsampling(modifyPlane(PixelComponents::V10, 	2),	{2, 1}) } };
	constexpr PixelFormat Y12U12V12_422		{ { PixelComponents::Y12,
									  			modifySubsampling(modifyPlane(PixelComponents::U12, 	1),	{2, 1}),
									  			modifySubsampling(modifyPlane(PixelComponents::V12, 	2),	{2, 1}) } };
	constexpr PixelFormat Y16U16V16_422		{ {	PixelComponents::Y16,
									  			modifySubsampling(modifyPlane(PixelComponents::U16, 	1),	{2, 1}),
									  			modifySubsampling(modifyPlane(PixelComponents::V16, 	2),	{2, 1}) } };

	

	constexpr PixelFormat Y8U8V8_420		{ {	PixelComponents::Y8,
									  			modifySubsampling(modifyPlane(PixelComponents::U8,		1),	{2, 2}),
									  			modifySubsampling(modifyPlane(PixelComponents::V8, 		2),	{2, 2}) } };
	constexpr PixelFormat Y10U10V10_420		{ { PixelComponents::Y10,
									  			modifySubsampling(modifyPlane(PixelComponents::U10, 	1),	{2, 2}),
									  			modifySubsampling(modifyPlane(PixelComponents::V10, 	2),	{2, 2}) } };
	constexpr PixelFormat Y12U12V12_420		{ { PixelComponents::Y12,
									  			modifySubsampling(modifyPlane(PixelComponents::U12, 	1),	{2, 2}),
									  			modifySubsampling(modifyPlane(PixelComponents::V12, 	2),	{2, 2}) } };
	constexpr PixelFormat Y16U16V16_420		{ {	PixelComponents::Y16,
									  			modifySubsampling(modifyPlane(PixelComponents::U16, 	1),	{2, 2}),
									  			modifySubsampling(modifyPlane(PixelComponents::V16, 	2),	{2, 2}) } };



	constexpr PixelFormat Y8U8V8_411		{ {	PixelComponents::Y8,
									  			modifySubsampling(modifyPlane(PixelComponents::U8,		1),	{4, 1}),
									  			modifySubsampling(modifyPlane(PixelComponents::V8, 		2),	{4, 1}) } };
	constexpr PixelFormat Y10U10V10_411		{ { PixelComponents::Y10,
									  			modifySubsampling(modifyPlane(PixelComponents::U10, 	1),	{4, 1}),
									  			modifySubsampling(modifyPlane(PixelComponents::V10, 	2),	{4, 1}) } };
	constexpr PixelFormat Y12U12V12_411		{ { PixelComponents::Y12,
									  			modifySubsampling(modifyPlane(PixelComponents::U12, 	1),	{4, 1}),
									  			modifySubsampling(modifyPlane(PixelComponents::V12, 	2),	{4, 1}) } };
	constexpr PixelFormat Y16U16V16_411		{ {	PixelComponents::Y16,
									  			modifySubsampling(modifyPlane(PixelComponents::U16, 	1),	{4, 1}),
									  			modifySubsampling(modifyPlane(PixelComponents::V16, 	2),	{4, 1}) } };



	constexpr PixelFormat Y8U8V8_410		{ {	PixelComponents::Y8,
									  			modifySubsampling(modifyPlane(PixelComponents::U8,		1),	{4, 2}),
									  			modifySubsampling(modifyPlane(PixelComponents::V8, 		2),	{4, 2}) } };
	constexpr PixelFormat Y10U10V10_410		{ { PixelComponents::Y10,
									  			modifySubsampling(modifyPlane(PixelComponents::U10, 	1),	{4, 2}),
									  			modifySubsampling(modifyPlane(PixelComponents::V10, 	2),	{4, 2}) } };
	constexpr PixelFormat Y12U12V12_410		{ { PixelComponents::Y12,
									  			modifySubsampling(modifyPlane(PixelComponents::U12, 	1),	{4, 2}),
									  			modifySubsampling(modifyPlane(PixelComponents::V12, 	2),	{4, 2}) } };
	constexpr PixelFormat Y16U16V16_410		{ {	PixelComponents::Y16,
									  			modifySubsampling(modifyPlane(PixelComponents::U16, 	1),	{4, 2}),
									  			modifySubsampling(modifyPlane(PixelComponents::V16, 	2),	{4, 2}) } };



	constexpr PixelFormat NV12		{ {	PixelComponents::Y8,
									  	modifySubsampling(modifyPlane(PixelComponents::U8,	1),	{2, 2}),
									  	modifySubsampling(modifyPlane(PixelComponents::V8, 	1),	{2, 2}) } };
	constexpr PixelFormat NV21		{ {	PixelComponents::Y10,
									  	modifySubsampling(modifyPlane(PixelComponents::U10, 	1),	{2, 2}),
									  	modifySubsampling(modifyPlane(PixelComponents::V10, 	1),	{2, 2}) } };
}
