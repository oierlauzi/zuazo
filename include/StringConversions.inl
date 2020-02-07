#include "StringConversions.h"

#include "Macros.h"

namespace Zuazo {

constexpr  std::string_view toString(PixelFormat format){

	switch(format){

	ZUAZO_ENUM2STR_CASE( PixelFormat, Y4X4_8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, X4Y4_8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, Y4A4_8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A4Y4_8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, X4R4G4B4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, X4B4G4R4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, X4G4R4B4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, X4G4B4R4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R4G4B4X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B4G4R4X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G4R4B4X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G4B4R4X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A4R4G4B4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A4B4G4R4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A4G4R4B4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A4G4B4R4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R4G4B4A4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B4G4R4A4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G4R4B4A4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G4B4R4A4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, X1R5G5B5_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, X1B5G5R5_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, X1G5R5B5_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, X1G5B5R5_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R5G5B5X1_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B5G5R5X1_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G5R5B5X1_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G5B5R5X1_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A1R5G5B5_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A1B5G5R5_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A1G5R5B5_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A1G5B5R5_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R5G5B5A1_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B5G5R5A1_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G5R5B5A1_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G5B5R5A1_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R5G6B5_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B5G6R5_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, Y8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, Y8A8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A8Y8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R8G8B8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B8G8R8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G8R8B8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G8B8R8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A8R8G8B8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A8B8G8R8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A8G8R8B8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A8G8B8R8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R8G8B8A8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B8G8R8A8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G8R8B8A8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G8B8R8A8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A8R8G8B8_32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A8B8G8R8_32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R8G8B8A8_32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B8G8R8A8_32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, X8R8G8B8_32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, X8B8G8R8_32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R8G8B8X8_32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B8G8R8X8_32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R8G8B8G8_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B8G8R8G8_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G8R8G8B8_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G8B8G8R8_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G8_B8_R8 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G8_B8_R8_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G8_B8_R8_S420 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G8_R8B8_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G8_B8R8_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G8_R8B8_S420 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G8_B8R8_S420 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, Y10X6_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, Y10X6A10X6_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A10X6Y10X6_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A10X6R10X6G10X6B10X6_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A10X6B10X6G10X6R10X6_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A10X6G10X6R10X6B10X6_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A10X6G10X6B10X6R10X6_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R10X6G10X6B10X6A10X6_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B10X6G10X6R10X6A10X6_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G10X6R10X6B10X6A10X6_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G10X6B10X6R10X6A10X6_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, X2R10G10B10_32 ) 
	ZUAZO_ENUM2STR_CASE( PixelFormat, X2B10G10R10_32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, X2G10R10B10_32 ) 
	ZUAZO_ENUM2STR_CASE( PixelFormat, X2G10B10R10_32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A2R10G10B10_32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A2B10G10R10_32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A2G10R10B10_32 ) 
	ZUAZO_ENUM2STR_CASE( PixelFormat, A2G10B10R10_32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R10X6G10X6B10X6G10X6_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B10X6G10X6R10X6G10X6_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G10X6R10X6G10X6B10X6_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G10X6B10X6G10X6R10X6_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G10X6_B10X6_R10X6_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G10X6_B10X6_R10X6_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G10X6_B10X6_R10X6_S420_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G10X6_R10X6B10X6_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G10X6_B10X6R10X6_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G10X6_R10X6B10X6_S420_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G10X6_B10X6R10X6_S420_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, Y12X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, Y12X4A12X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A12X4Y12X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A12X4R12X4G12X4B12X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A12X4B12X4G12X4R12X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A12X4G12X4R12X4B12X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A12X4G12X4B12X4R12X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R12X4G12X4B12X4A12X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B12X4G12X4R12X4A12X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G12X4R12X4B12X4A12X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G12X4B12X4R12X4A12X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R12X4G12X4B12X4G12X4_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B12X4G12X4R12X4G12X4_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G12X4R12X4G12X4B12X4_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G12X4B12X4G12X4R12X4_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G12X4_B12X4_R12X4_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G12X4_B12X4_R12X4_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G12X4_B12X4_R12X4_S420_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G12X4_R12X4B12X4_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G12X4_B12X4R12X4_S422_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G12X4_R12X4B12X4_S420_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G12X4_B12X4R12X4_S420_16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, Y16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, Y16A16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A16Y16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R16G16B16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B16G16R16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G16R16B16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G16B16R16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A16R16G16B16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A16B16G16R16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A16G16R16B16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A16G16B16R16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R16G16B16A16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B16G16R16A16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G16R16B16A16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G16B16R16A16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R16G16B16G16_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B16G16R16G16_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G16R16G16B16_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G16B16G16R16_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G16_B16_R16 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G16_B16_R16_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G16_B16_R16_S420 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G16_R16B16_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G16_B16R16_S422 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G16_R16B16_S420 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G16_B16R16_S420 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, Y32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, Y32A32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A32Y32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R32G32B32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B32G32R32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G32R32B32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G32B32R32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A32R32G32B32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A32B32G32R32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A32G32R32B32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A32G32B32R32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R32G32B32A32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B32G32R32A32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G32R32B32A32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G32B32R32A32 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, Y64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, Y64A64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A64Y64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R64G64B64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B64G64R64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G64R64B64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G64B64R64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A64R64G64B64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A64B64G64R64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A64G64R64B64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, A64G64B64R64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, R64G64B64A64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, B64G64R64A64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G64R64B64A64 )
	ZUAZO_ENUM2STR_CASE( PixelFormat, G64B64R64A64 )

	default: return "";
	}
}

}