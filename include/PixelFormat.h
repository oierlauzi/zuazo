#pragma once

#include <array>
#include <bitset>

struct PixelComponent{
    enum PixelComponentType{
        NONE        =0,
        RED         ,
        R           =RED,
        GREEN       ,
        G           =GREEN
        BLUE        ,
        B           =BLUE,
        ALPHA       ,
        A           =ALPHA,
        LUMINANCE   ,
        Y           =LUMINANCE,
        U           ,
        V           ,
    };

    enum Flag {
        FLOATING_IEEE_754,
        FLAGS_COUNT
    };

    PixelComponentType          type;
    uint                        depth;
    uint                        plane;
    std::bitset<FLAGS_COUNT>    flags;
    Math::Rational32_t          subsampling;

    constexpr PixelComponent();
    constexpr PixelComponent(PixelComponentType type, uint depth, uint plane = 0, uint32_t flags = 0, Math::Rational32_t subs = 1);
    PixelComponent(const PixelComponent& other)=default;
    ~PixelComponent()=default;
}

constexpr size_t MAX_PIXEL_COMPONENTS = 8;
typedef std::array<PixelComponent, MAX_PIXEL_COMPONENTS> PixelFormat;

namespace 

}

#include "PixelFormat.inl"