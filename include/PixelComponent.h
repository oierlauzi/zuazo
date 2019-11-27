#pragma once

#include "Math/Rational.h"
#include "Macros.h"

#include <bitset>

namespace Zuazo {

struct PixelComponent{
	enum class Type {
		NONE        =0,				///Does not represent any pixel component
		N			=NONE,			///Short form of NONE
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
		IEEE_754	=ZUAZO_BIT(0), 	///Floating point, IEEE_754 standard 

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

} //namespace Zuazo

#include "PixelComponent.inl"

namespace Zuazo::PixelComponents {
	constexpr PixelComponent NONE   = { PixelComponent::Type::NONE,	0  };
	constexpr PixelComponent NONE1  = { PixelComponent::Type::NONE,	1  };
	constexpr PixelComponent NONE2  = { PixelComponent::Type::NONE,	2  };
	constexpr PixelComponent NONE3  = { PixelComponent::Type::NONE,	3  };
	constexpr PixelComponent NONE4  = { PixelComponent::Type::NONE,	4  };
	constexpr PixelComponent NONE5  = { PixelComponent::Type::NONE,	5  };
	constexpr PixelComponent NONE6  = { PixelComponent::Type::NONE,	6  };
	constexpr PixelComponent NONE7  = { PixelComponent::Type::NONE,	7  };
	constexpr PixelComponent NONE8  = { PixelComponent::Type::NONE,	8  };
	constexpr PixelComponent NONE10 = { PixelComponent::Type::NONE,	10 };
	constexpr PixelComponent NONE12 = { PixelComponent::Type::NONE,	12 };
	constexpr PixelComponent NONE14 = { PixelComponent::Type::NONE,	14 };
	constexpr PixelComponent NONE16 = { PixelComponent::Type::NONE,	16 };
	constexpr PixelComponent NONE24 = { PixelComponent::Type::NONE,	24 };
	constexpr PixelComponent NONE32 = { PixelComponent::Type::NONE,	32 };
	constexpr PixelComponent NONE48 = { PixelComponent::Type::NONE,	48 };
	constexpr PixelComponent NONE64 = { PixelComponent::Type::NONE,	64 };

	constexpr PixelComponent R1     = { PixelComponent::Type::R, 	1  	};
	constexpr PixelComponent R2     = { PixelComponent::Type::R, 	2  	};
	constexpr PixelComponent R3     = { PixelComponent::Type::R, 	3  	};
	constexpr PixelComponent R4     = { PixelComponent::Type::R, 	4  	};
	constexpr PixelComponent R5     = { PixelComponent::Type::R, 	5  	};
	constexpr PixelComponent R6     = { PixelComponent::Type::R, 	6  	};
	constexpr PixelComponent R7     = { PixelComponent::Type::R, 	7  	};
	constexpr PixelComponent R8     = { PixelComponent::Type::R, 	8  	};
	constexpr PixelComponent R10    = { PixelComponent::Type::R, 	10 	};
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
	constexpr PixelComponent G10    = { PixelComponent::Type::G, 	10 	};
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
	constexpr PixelComponent B10    = { PixelComponent::Type::B, 	10 	};
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
	constexpr PixelComponent A10    = { PixelComponent::Type::A, 	10 	};
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
	constexpr PixelComponent Y10    = { PixelComponent::Type::Y, 	10 	};
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
	constexpr PixelComponent U10    = { PixelComponent::Type::U, 	10 	};
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
	constexpr PixelComponent V10    = { PixelComponent::Type::V, 	10 	};
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