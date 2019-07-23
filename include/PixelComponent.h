#pragma once

#include "Math/Rational.h"

#include <bitset>

namespace Zuazo {

struct PixelComponent{
	enum PixelComponentType{
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

		COMPONENT_TYPE_COUNT
	};

	enum Flag : uint {
		IEEE_754, 	///Floating point, IEEE_754 standard 
		BAYER,		///Bayer pattern

		//ADD HERE

		FLAG_COUNT
	};
	typedef std::bitset<FLAG_COUNT> Flags;

	struct Subsampling {
		typedef Math::Rational32_t Rational;
		Rational x, y;

		constexpr Subsampling();
		constexpr Subsampling(uint16_t x, uint16_t y);
		constexpr Subsampling(const Subsampling& other) = default;
		~Subsampling() = default;
	};

	PixelComponentType			type;
	uint8_t						depth;
	uint8_t						plane;
	Subsampling					subsampling;
	Flags						flags;

	constexpr PixelComponent();
	constexpr PixelComponent(uint depth);
	constexpr PixelComponent(PixelComponentType type, uint depth, uint plane = 0, const Subsampling& subs =  Subsampling(), Flags flags = 0);
	constexpr PixelComponent(const PixelComponent& other, uint plane, const Subsampling& subs = Subsampling());
	constexpr PixelComponent(const PixelComponent& other) = default;
	~PixelComponent() = default;

	constexpr operator bool() const;
};

} //namespace Zuazo

#include "PixelComponent.inl"

namespace Zuazo::PixelComponents {

	constexpr auto IEEE_754_BIT = ZUAZO_BIT(PixelComponent::IEEE_754);
	constexpr auto BAYER_BIT = ZUAZO_BIT(PixelComponent::BAYER);

	constexpr auto NO_SUBSAMPLING = PixelComponent::Subsampling();

	constexpr PixelComponent NONE   = PixelComponent();
	constexpr PixelComponent NONE1  = PixelComponent(1);
	constexpr PixelComponent NONE2  = PixelComponent(2);
	constexpr PixelComponent NONE3  = PixelComponent(3);
	constexpr PixelComponent NONE4  = PixelComponent(4);
	constexpr PixelComponent NONE5  = PixelComponent(5);
	constexpr PixelComponent NONE6  = PixelComponent(6);
	constexpr PixelComponent NONE7  = PixelComponent(7);
	constexpr PixelComponent NONE8  = PixelComponent(8);
	constexpr PixelComponent NONE10 = PixelComponent(10);
	constexpr PixelComponent NONE12 = PixelComponent(12);
	constexpr PixelComponent NONE14 = PixelComponent(14);
	constexpr PixelComponent NONE16 = PixelComponent(16);
	constexpr PixelComponent NONE24 = PixelComponent(24);
	constexpr PixelComponent NONE32 = PixelComponent(32);
	constexpr PixelComponent NONE48 = PixelComponent(48);
	constexpr PixelComponent NONE64 = PixelComponent(64);

	constexpr PixelComponent R1     = PixelComponent(PixelComponent::R, 1,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R2     = PixelComponent(PixelComponent::R, 2,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R3     = PixelComponent(PixelComponent::R, 3,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R4     = PixelComponent(PixelComponent::R, 4,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R5     = PixelComponent(PixelComponent::R, 5,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R6     = PixelComponent(PixelComponent::R, 6,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R7     = PixelComponent(PixelComponent::R, 7,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R8     = PixelComponent(PixelComponent::R, 8,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R10    = PixelComponent(PixelComponent::R, 10, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R12    = PixelComponent(PixelComponent::R, 12, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R14    = PixelComponent(PixelComponent::R, 14, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R16    = PixelComponent(PixelComponent::R, 16, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R16f   = PixelComponent(PixelComponent::R, 16, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent R24    = PixelComponent(PixelComponent::R, 24, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R32    = PixelComponent(PixelComponent::R, 32, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R32f   = PixelComponent(PixelComponent::R, 32, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent R64    = PixelComponent(PixelComponent::R, 64, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent R64f   = PixelComponent(PixelComponent::R, 64, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );

	constexpr PixelComponent G1     = PixelComponent(PixelComponent::G, 1,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G2     = PixelComponent(PixelComponent::G, 2,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G3     = PixelComponent(PixelComponent::G, 3,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G4     = PixelComponent(PixelComponent::G, 4,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G5     = PixelComponent(PixelComponent::G, 5,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G6     = PixelComponent(PixelComponent::G, 6,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G7     = PixelComponent(PixelComponent::G, 7,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G8     = PixelComponent(PixelComponent::G, 8,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G10    = PixelComponent(PixelComponent::G, 10, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G12    = PixelComponent(PixelComponent::G, 12, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G14    = PixelComponent(PixelComponent::G, 14, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G16    = PixelComponent(PixelComponent::G, 16, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G16f   = PixelComponent(PixelComponent::G, 16, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent G24    = PixelComponent(PixelComponent::G, 24, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G32    = PixelComponent(PixelComponent::G, 32, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G32f   = PixelComponent(PixelComponent::G, 32, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent G64    = PixelComponent(PixelComponent::G, 64, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent G64f   = PixelComponent(PixelComponent::G, 64, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent G1B    = PixelComponent(PixelComponent::G, 1,  0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G2B    = PixelComponent(PixelComponent::G, 2,  0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G3B    = PixelComponent(PixelComponent::G, 3,  0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G4B    = PixelComponent(PixelComponent::G, 4,  0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G5B    = PixelComponent(PixelComponent::G, 5,  0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G6B    = PixelComponent(PixelComponent::G, 6,  0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G7B    = PixelComponent(PixelComponent::G, 7,  0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G8B    = PixelComponent(PixelComponent::G, 8,  0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G10B   = PixelComponent(PixelComponent::G, 10, 0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G12B   = PixelComponent(PixelComponent::G, 12, 0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G14B   = PixelComponent(PixelComponent::G, 14, 0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G16B   = PixelComponent(PixelComponent::G, 16, 0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G16fB  = PixelComponent(PixelComponent::G, 16, 0,  NO_SUBSAMPLING,  BAYER_BIT | IEEE_754_BIT );
	constexpr PixelComponent G24B   = PixelComponent(PixelComponent::G, 24, 0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G32B   = PixelComponent(PixelComponent::G, 32, 0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G32fB  = PixelComponent(PixelComponent::G, 32, 0,  NO_SUBSAMPLING,  BAYER_BIT | IEEE_754_BIT );
	constexpr PixelComponent G64B   = PixelComponent(PixelComponent::G, 64, 0,  NO_SUBSAMPLING,  BAYER_BIT );
	constexpr PixelComponent G64fB  = PixelComponent(PixelComponent::G, 64, 0,  NO_SUBSAMPLING,  BAYER_BIT | IEEE_754_BIT );

	constexpr PixelComponent B1     = PixelComponent(PixelComponent::B, 1,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B2     = PixelComponent(PixelComponent::B, 2,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B3     = PixelComponent(PixelComponent::B, 3,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B4     = PixelComponent(PixelComponent::B, 4,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B5     = PixelComponent(PixelComponent::B, 5,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B6     = PixelComponent(PixelComponent::B, 6,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B7     = PixelComponent(PixelComponent::B, 7,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B8     = PixelComponent(PixelComponent::B, 8,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B10    = PixelComponent(PixelComponent::B, 10, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B12    = PixelComponent(PixelComponent::B, 12, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B14    = PixelComponent(PixelComponent::B, 14, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B16    = PixelComponent(PixelComponent::B, 16, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B16f   = PixelComponent(PixelComponent::B, 16, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent B24    = PixelComponent(PixelComponent::B, 24, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B32    = PixelComponent(PixelComponent::B, 32, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B32f   = PixelComponent(PixelComponent::B, 32, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent B64    = PixelComponent(PixelComponent::B, 64, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent B64f   = PixelComponent(PixelComponent::B, 64, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );

	constexpr PixelComponent A1     = PixelComponent(PixelComponent::A, 1,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A2     = PixelComponent(PixelComponent::A, 2,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A3     = PixelComponent(PixelComponent::A, 3,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A4     = PixelComponent(PixelComponent::A, 4,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A5     = PixelComponent(PixelComponent::A, 5,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A6     = PixelComponent(PixelComponent::A, 6,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A7     = PixelComponent(PixelComponent::A, 7,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A8     = PixelComponent(PixelComponent::A, 8,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A10    = PixelComponent(PixelComponent::A, 10, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A12    = PixelComponent(PixelComponent::A, 12, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A14    = PixelComponent(PixelComponent::A, 14, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A16    = PixelComponent(PixelComponent::A, 16, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A16f   = PixelComponent(PixelComponent::A, 16, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent A24    = PixelComponent(PixelComponent::A, 24, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A32    = PixelComponent(PixelComponent::A, 32, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A32f   = PixelComponent(PixelComponent::A, 32, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent A64    = PixelComponent(PixelComponent::A, 64, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent A64f   = PixelComponent(PixelComponent::A, 64, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );

	constexpr PixelComponent Y1     = PixelComponent(PixelComponent::Y, 1,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y2     = PixelComponent(PixelComponent::Y, 2,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y3     = PixelComponent(PixelComponent::Y, 3,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y4     = PixelComponent(PixelComponent::Y, 4,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y5     = PixelComponent(PixelComponent::Y, 5,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y6     = PixelComponent(PixelComponent::Y, 6,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y7     = PixelComponent(PixelComponent::Y, 7,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y8     = PixelComponent(PixelComponent::Y, 8,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y10    = PixelComponent(PixelComponent::Y, 10, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y12    = PixelComponent(PixelComponent::Y, 12, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y14    = PixelComponent(PixelComponent::Y, 14, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y16    = PixelComponent(PixelComponent::Y, 16, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y16f   = PixelComponent(PixelComponent::Y, 16, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent Y24    = PixelComponent(PixelComponent::Y, 24, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y32    = PixelComponent(PixelComponent::Y, 32, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y32f   = PixelComponent(PixelComponent::Y, 32, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent Y64    = PixelComponent(PixelComponent::Y, 64, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent Y64f   = PixelComponent(PixelComponent::Y, 64, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );

	constexpr PixelComponent U1     = PixelComponent(PixelComponent::U, 1,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U2     = PixelComponent(PixelComponent::U, 2,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U3     = PixelComponent(PixelComponent::U, 3,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U4     = PixelComponent(PixelComponent::U, 4,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U5     = PixelComponent(PixelComponent::U, 5,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U6     = PixelComponent(PixelComponent::U, 6,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U7     = PixelComponent(PixelComponent::U, 7,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U8     = PixelComponent(PixelComponent::U, 8,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U10    = PixelComponent(PixelComponent::U, 10, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U12    = PixelComponent(PixelComponent::U, 12, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U14    = PixelComponent(PixelComponent::U, 14, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U16    = PixelComponent(PixelComponent::U, 16, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U16f   = PixelComponent(PixelComponent::U, 16, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent U24    = PixelComponent(PixelComponent::U, 24, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U32    = PixelComponent(PixelComponent::U, 32, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U32f   = PixelComponent(PixelComponent::U, 32, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent U64    = PixelComponent(PixelComponent::U, 64, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent U64f   = PixelComponent(PixelComponent::U, 64, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );

	constexpr PixelComponent V1     = PixelComponent(PixelComponent::V, 1,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V2     = PixelComponent(PixelComponent::V, 2,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V3     = PixelComponent(PixelComponent::V, 3,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V4     = PixelComponent(PixelComponent::V, 4,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V5     = PixelComponent(PixelComponent::V, 5,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V6     = PixelComponent(PixelComponent::V, 6,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V7     = PixelComponent(PixelComponent::V, 7,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V8     = PixelComponent(PixelComponent::V, 8,  0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V10    = PixelComponent(PixelComponent::V, 10, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V12    = PixelComponent(PixelComponent::V, 12, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V14    = PixelComponent(PixelComponent::V, 14, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V16    = PixelComponent(PixelComponent::V, 16, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V16f   = PixelComponent(PixelComponent::V, 16, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent V24    = PixelComponent(PixelComponent::V, 24, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V32    = PixelComponent(PixelComponent::V, 32, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V32f   = PixelComponent(PixelComponent::V, 32, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );
	constexpr PixelComponent V64    = PixelComponent(PixelComponent::V, 64, 0,  NO_SUBSAMPLING,  0   );
	constexpr PixelComponent V64f   = PixelComponent(PixelComponent::V, 64, 0,  NO_SUBSAMPLING,  IEEE_754_BIT );

}