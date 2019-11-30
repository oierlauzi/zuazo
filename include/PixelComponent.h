#pragma once

#include "Math/Rational.h"
#include "Macros.h"

#include <bitset>

namespace Zuazo {

struct PixelComponent{
	enum class Type {
		NONE        =0,				///Does not represent any pixel component
		X			=NONE,			///Short form of NONE
		RED         ,				///Represents a red subpixel
		R           =RED,			///Short form of RED
		GREEN       ,				///Represents a green subpixel
		G           =GREEN,			///Short form of GREEN
		BLUE        ,				///Represents a blue subpixel
		B           =BLUE,			///Short form of BLUE
		ALPHA       ,				///Represents the alpha (transparency) component of a pixel
		A           =ALPHA,			///Short form of ALPHA
		LUMINANCE   ,				///Represents the luminance component of a pixel
		Y           =LUMINANCE,		///Short form of LUMINANCE
		U           ,				///Cb component
		V           ,				///Cr component 

		//ADD HERE

		COUNT
	};

	enum class Flags {
		IEEE_754			= ZUAZO_BIT(0), 	///Floating point, IEEE_754 standard 
		FORCE_LITTLE_ENDIAN = ZUAZO_BIT(1),		///Always stored as little endian\
												@note If no endianess is specified, system endianess\
												will be used

		FORCE_BIG_ENDIAN	= ZUAZO_BIT(2)		///Always stored as big endian.\
												@note If no endianess is specified, system endianess\
												will be used

		//ADD HERE
	};

	struct Subsampling {
		uint16_t x = 1, y = 1; 
	};

	static constexpr Subsampling NO_SUBSAMPLING {1, 1};

	Type						type = Type::NONE;
	uint32_t					depth = 0;
	uint32_t					plane = 0;
	Subsampling					subsampling = NO_SUBSAMPLING;
	Flags						flags = {};

	constexpr PixelComponent() = default;
	constexpr PixelComponent(const PixelComponent& other) = default;
	~PixelComponent() = default;

	constexpr PixelComponent& operator=(const PixelComponent& other) = default;

	constexpr bool operator==(const PixelComponent& other) const;
	constexpr bool operator!=(const PixelComponent& other) const;

	constexpr operator bool() const;
};

ZUAZO_ENUM_FLAG_OPERATORS(PixelComponent::Flags)


constexpr PixelComponent modifyType(const PixelComponent& component, PixelComponent::Type newType);
constexpr PixelComponent modifyDepth(const PixelComponent& component, uint32_t newDepth);
constexpr PixelComponent modifyPlane(const PixelComponent& component, uint32_t newPlane);
constexpr PixelComponent modifySubsampling(const PixelComponent& component, PixelComponent::Subsampling newSubsampling);
constexpr PixelComponent modifyFlags(const PixelComponent& component, PixelComponent::Flags newFlags);
constexpr bool areCompatible(PixelComponent a, PixelComponent b);

} //namespace Zuazo

#include "PixelComponent.inl"

namespace Zuazo::PixelComponents {
	constexpr PixelComponent NONE   = { PixelComponent::Type::X,	0	};

	constexpr PixelComponent X1 	= { PixelComponent::Type::X,	1 	};
	constexpr PixelComponent X2 	= { PixelComponent::Type::X,	2 	};
	constexpr PixelComponent X3 	= { PixelComponent::Type::X,	3 	};
	constexpr PixelComponent X4 	= { PixelComponent::Type::X,	4 	};
	constexpr PixelComponent X5 	= { PixelComponent::Type::X,	5 	};
	constexpr PixelComponent X6 	= { PixelComponent::Type::X,	6 	};
	constexpr PixelComponent X7 	= { PixelComponent::Type::X,	7 	};
	constexpr PixelComponent X8 	= { PixelComponent::Type::X,	8 	};
	constexpr PixelComponent X9		= { PixelComponent::Type::X,	9 	};
	constexpr PixelComponent X10	= { PixelComponent::Type::X,	10	};
	constexpr PixelComponent X11	= { PixelComponent::Type::X,	11	};
	constexpr PixelComponent X12	= { PixelComponent::Type::X,	12	};
	constexpr PixelComponent X14	= { PixelComponent::Type::X,	14	};
	constexpr PixelComponent X16	= { PixelComponent::Type::X,	16	};
	constexpr PixelComponent X24	= { PixelComponent::Type::X,	24	};
	constexpr PixelComponent X32	= { PixelComponent::Type::X,	32	};
	constexpr PixelComponent X48	= { PixelComponent::Type::X,	48	};
	constexpr PixelComponent X64	= { PixelComponent::Type::X,	64	};

	constexpr PixelComponent R1     = { PixelComponent::Type::R, 	1  	};
	constexpr PixelComponent R2     = { PixelComponent::Type::R, 	2  	};
	constexpr PixelComponent R3     = { PixelComponent::Type::R, 	3  	};
	constexpr PixelComponent R4     = { PixelComponent::Type::R, 	4  	};
	constexpr PixelComponent R5     = { PixelComponent::Type::R, 	5  	};
	constexpr PixelComponent R6     = { PixelComponent::Type::R, 	6  	};
	constexpr PixelComponent R7     = { PixelComponent::Type::R, 	7  	};
	constexpr PixelComponent R8     = { PixelComponent::Type::R, 	8  	};
	constexpr PixelComponent R9		= { PixelComponent::Type::R,	9 	};
	constexpr PixelComponent R10	= { PixelComponent::Type::R,	10	};
	constexpr PixelComponent R11	= { PixelComponent::Type::R,	11	};
	constexpr PixelComponent R12    = { PixelComponent::Type::R, 	12 	};
	constexpr PixelComponent R14    = { PixelComponent::Type::R, 	14 	};
	constexpr PixelComponent R16    = { PixelComponent::Type::R, 	16 	};
	constexpr PixelComponent R16f   = modifyFlags(R16, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent R20    = { PixelComponent::Type::R, 	20 	};
	constexpr PixelComponent R24    = { PixelComponent::Type::R, 	24 	};
	constexpr PixelComponent R32    = { PixelComponent::Type::R, 	32 	};
	constexpr PixelComponent R32f   = modifyFlags(R32, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent R64    = { PixelComponent::Type::R, 	64 	};
	constexpr PixelComponent R64f   = modifyFlags(R64, PixelComponent::Flags::IEEE_754);

	constexpr PixelComponent G1     = { PixelComponent::Type::G, 	1  	};
	constexpr PixelComponent G2     = { PixelComponent::Type::G, 	2  	};
	constexpr PixelComponent G3     = { PixelComponent::Type::G, 	3  	};
	constexpr PixelComponent G4     = { PixelComponent::Type::G, 	4  	};
	constexpr PixelComponent G5     = { PixelComponent::Type::G, 	5  	};
	constexpr PixelComponent G6     = { PixelComponent::Type::G, 	6  	};
	constexpr PixelComponent G7     = { PixelComponent::Type::G, 	7  	};
	constexpr PixelComponent G8     = { PixelComponent::Type::G, 	8  	};
	constexpr PixelComponent G9		= { PixelComponent::Type::G,	9 	};
	constexpr PixelComponent G10	= { PixelComponent::Type::G,	10	};
	constexpr PixelComponent G11	= { PixelComponent::Type::G,	11	};
	constexpr PixelComponent G12    = { PixelComponent::Type::G, 	12 	};
	constexpr PixelComponent G14    = { PixelComponent::Type::G, 	14 	};
	constexpr PixelComponent G16    = { PixelComponent::Type::G, 	16 	};
	constexpr PixelComponent G16f   = modifyFlags(G16, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent G20    = { PixelComponent::Type::G, 	20 	};
	constexpr PixelComponent G24    = { PixelComponent::Type::G, 	24 	};
	constexpr PixelComponent G32    = { PixelComponent::Type::G, 	32 	};
	constexpr PixelComponent G32f   = modifyFlags(G32, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent G64    = { PixelComponent::Type::G, 	64 	};
	constexpr PixelComponent G64f   = modifyFlags(G64, PixelComponent::Flags::IEEE_754);

	constexpr PixelComponent B1     = { PixelComponent::Type::B, 	1  	};
	constexpr PixelComponent B2     = { PixelComponent::Type::B, 	2  	};
	constexpr PixelComponent B3     = { PixelComponent::Type::B, 	3  	};
	constexpr PixelComponent B4     = { PixelComponent::Type::B, 	4  	};
	constexpr PixelComponent B5     = { PixelComponent::Type::B, 	5  	};
	constexpr PixelComponent B6     = { PixelComponent::Type::B, 	6  	};
	constexpr PixelComponent B7     = { PixelComponent::Type::B, 	7  	};
	constexpr PixelComponent B8     = { PixelComponent::Type::B, 	8  	};
	constexpr PixelComponent B9		= { PixelComponent::Type::B,	9 	};
	constexpr PixelComponent B10	= { PixelComponent::Type::B,	10	};
	constexpr PixelComponent B11	= { PixelComponent::Type::B,	11	};
	constexpr PixelComponent B12    = { PixelComponent::Type::B, 	12 	};
	constexpr PixelComponent B14    = { PixelComponent::Type::B, 	14 	};
	constexpr PixelComponent B16    = { PixelComponent::Type::B, 	16 	};
	constexpr PixelComponent B16f   = modifyFlags(B16, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent B20    = { PixelComponent::Type::B, 	20 	};
	constexpr PixelComponent B24    = { PixelComponent::Type::B, 	24 	};
	constexpr PixelComponent B32    = { PixelComponent::Type::B, 	32 	};
	constexpr PixelComponent B32f   = modifyFlags(B32, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent B64    = { PixelComponent::Type::B, 	64 	};
	constexpr PixelComponent B64f   = modifyFlags(B64, PixelComponent::Flags::IEEE_754);

	constexpr PixelComponent A1     = { PixelComponent::Type::A, 	1  	};
	constexpr PixelComponent A2     = { PixelComponent::Type::A, 	2  	};
	constexpr PixelComponent A3     = { PixelComponent::Type::A, 	3  	};
	constexpr PixelComponent A4     = { PixelComponent::Type::A, 	4  	};
	constexpr PixelComponent A5     = { PixelComponent::Type::A, 	5  	};
	constexpr PixelComponent A6     = { PixelComponent::Type::A, 	6  	};
	constexpr PixelComponent A7     = { PixelComponent::Type::A, 	7  	};
	constexpr PixelComponent A8     = { PixelComponent::Type::A, 	8  	};
	constexpr PixelComponent A9		= { PixelComponent::Type::A,	9 	};
	constexpr PixelComponent A10	= { PixelComponent::Type::A,	10	};
	constexpr PixelComponent A11	= { PixelComponent::Type::A,	11	};
	constexpr PixelComponent A12    = { PixelComponent::Type::A, 	12 	};
	constexpr PixelComponent A14    = { PixelComponent::Type::A, 	14 	};
	constexpr PixelComponent A16    = { PixelComponent::Type::A, 	16 	};
	constexpr PixelComponent A16f   = modifyFlags(A16, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent A20    = { PixelComponent::Type::A, 	20 	};
	constexpr PixelComponent A24    = { PixelComponent::Type::A, 	24 	};
	constexpr PixelComponent A32    = { PixelComponent::Type::A, 	32 	};
	constexpr PixelComponent A32f   = modifyFlags(A32, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent A64    = { PixelComponent::Type::A, 	64 	};
	constexpr PixelComponent A64f   = modifyFlags(A64, PixelComponent::Flags::IEEE_754);

	constexpr PixelComponent Y1     = { PixelComponent::Type::Y, 	1  	};
	constexpr PixelComponent Y2     = { PixelComponent::Type::Y, 	2  	};
	constexpr PixelComponent Y3     = { PixelComponent::Type::Y, 	3  	};
	constexpr PixelComponent Y4     = { PixelComponent::Type::Y, 	4  	};
	constexpr PixelComponent Y5     = { PixelComponent::Type::Y, 	5  	};
	constexpr PixelComponent Y6     = { PixelComponent::Type::Y, 	6  	};
	constexpr PixelComponent Y7     = { PixelComponent::Type::Y, 	7  	};
	constexpr PixelComponent Y8     = { PixelComponent::Type::Y, 	8  	};
	constexpr PixelComponent Y9		= { PixelComponent::Type::Y,	9 	};
	constexpr PixelComponent Y10	= { PixelComponent::Type::Y,	10	};
	constexpr PixelComponent Y11	= { PixelComponent::Type::Y,	11	};
	constexpr PixelComponent Y12    = { PixelComponent::Type::Y, 	12 	};
	constexpr PixelComponent Y14    = { PixelComponent::Type::Y, 	14 	};
	constexpr PixelComponent Y16    = { PixelComponent::Type::Y, 	16 	};
	constexpr PixelComponent Y16f   = modifyFlags(Y16, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent Y20    = { PixelComponent::Type::Y, 	20 	};
	constexpr PixelComponent Y24    = { PixelComponent::Type::Y, 	24 	};
	constexpr PixelComponent Y32    = { PixelComponent::Type::Y, 	32 	};
	constexpr PixelComponent Y32f   = modifyFlags(Y32, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent Y64    = { PixelComponent::Type::Y, 	64 	};
	constexpr PixelComponent Y64f   = modifyFlags(Y64, PixelComponent::Flags::IEEE_754);

	constexpr PixelComponent U1     = { PixelComponent::Type::U, 	1  	};
	constexpr PixelComponent U2     = { PixelComponent::Type::U, 	2  	};
	constexpr PixelComponent U3     = { PixelComponent::Type::U, 	3  	};
	constexpr PixelComponent U4     = { PixelComponent::Type::U, 	4  	};
	constexpr PixelComponent U5     = { PixelComponent::Type::U, 	5  	};
	constexpr PixelComponent U6     = { PixelComponent::Type::U, 	6  	};
	constexpr PixelComponent U7     = { PixelComponent::Type::U, 	7  	};
	constexpr PixelComponent U8     = { PixelComponent::Type::U, 	8  	};
	constexpr PixelComponent U9		= { PixelComponent::Type::U,	9 	};
	constexpr PixelComponent U10	= { PixelComponent::Type::U,	10	};
	constexpr PixelComponent U11	= { PixelComponent::Type::U,	11	};
	constexpr PixelComponent U12    = { PixelComponent::Type::U, 	12 	};
	constexpr PixelComponent U14    = { PixelComponent::Type::U, 	14 	};
	constexpr PixelComponent U16    = { PixelComponent::Type::U, 	16 	};
	constexpr PixelComponent U16f   = modifyFlags(U16, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent U20    = { PixelComponent::Type::U, 	20 	};
	constexpr PixelComponent U24    = { PixelComponent::Type::U, 	24 	};
	constexpr PixelComponent U32    = { PixelComponent::Type::U, 	32 	};
	constexpr PixelComponent U32f   = modifyFlags(U32, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent U64    = { PixelComponent::Type::U, 	64 	};
	constexpr PixelComponent U64f   = modifyFlags(U64, PixelComponent::Flags::IEEE_754);

	constexpr PixelComponent V1     = { PixelComponent::Type::V, 	1  	};
	constexpr PixelComponent V2     = { PixelComponent::Type::V, 	2  	};
	constexpr PixelComponent V3     = { PixelComponent::Type::V, 	3  	};
	constexpr PixelComponent V4     = { PixelComponent::Type::V, 	4  	};
	constexpr PixelComponent V5     = { PixelComponent::Type::V, 	5  	};
	constexpr PixelComponent V6     = { PixelComponent::Type::V, 	6  	};
	constexpr PixelComponent V7     = { PixelComponent::Type::V, 	7  	};
	constexpr PixelComponent V8     = { PixelComponent::Type::V, 	8  	};
	constexpr PixelComponent V9		= { PixelComponent::Type::V,	9 	};
	constexpr PixelComponent V10	= { PixelComponent::Type::V,	10	};
	constexpr PixelComponent V11	= { PixelComponent::Type::V,	11	};
	constexpr PixelComponent V12    = { PixelComponent::Type::V, 	12 	};
	constexpr PixelComponent V14    = { PixelComponent::Type::V, 	14 	};
	constexpr PixelComponent V16    = { PixelComponent::Type::V, 	16 	};
	constexpr PixelComponent V16f   = modifyFlags(V16, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent V20    = { PixelComponent::Type::V, 	20 	};
	constexpr PixelComponent V24    = { PixelComponent::Type::V, 	24 	};
	constexpr PixelComponent V32    = { PixelComponent::Type::V, 	32 	};
	constexpr PixelComponent V32f   = modifyFlags(V32, PixelComponent::Flags::IEEE_754);
	constexpr PixelComponent V64    = { PixelComponent::Type::V, 	64 	};
	constexpr PixelComponent V64f   = modifyFlags(V64, PixelComponent::Flags::IEEE_754);

}