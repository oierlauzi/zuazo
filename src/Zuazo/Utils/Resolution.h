#pragma once

#include <sys/types.h>

#include "PixelTypes.h"

namespace Zuazo::Utils{

struct Resolution{
	u_int32_t	width;
	u_int32_t	height;

	constexpr Resolution():
		width(0),
		height(0)
	{
	}

	constexpr Resolution(u_int32_t width, u_int32_t height) :
		width(width),
		height(height)
	{
	}

	constexpr size_t size(size_t pixSize=1) const{
		return width * height * pixSize;
	}

	constexpr operator bool() const{
		return width && height;
	}

	constexpr int operator==(const Resolution& right)const{
        return right.width==width && right.height==height;
    }

	constexpr int operator!=(const Resolution& right)const{
        return right.width!=width || right.height!=height;
    }

	constexpr int operator<(const Resolution& right)const{
        int result=0;
        if(width < right.width)
        	result|=0x1;
        if(height < right.height)
        	result|=0x2;
        return result;
    }

	constexpr int operator<=(const Resolution& right)const{
        int result=0;
        if(width <= right.width)
        	result|= 0x1;
        if(height <= right.height)
        	result|= 0x2;
        return result;
    }

	constexpr int operator>(const Resolution& right)const{
    	return ~operator<=(right);
    }

	constexpr int operator>=(const Resolution& right)const{
    	return ~operator<(right);
    }
};

}
