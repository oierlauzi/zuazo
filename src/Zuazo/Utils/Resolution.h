#pragma once

#include <sys/types.h>

#include "PixelTypes.h"

namespace Zuazo::Utils{

struct Resolution{
	u_int32_t	width;
	u_int32_t	height;

	Resolution(){
		width=0;
		height=0;
	}

	Resolution(u_int32_t width, u_int32_t height){
		this->width=width;
		this->height=height;
	}

	size_t	getSize(size_t pixSize){
		return width* height* pixSize;
	}

	template<PixelTypes type>
	size_t	getSize(){
		return getSize(PIXEL_SIZE<type>);
	}

	operator bool() const{
		return width && height;
	}

    int operator==(const Resolution& right)const{
        return right.width==width && right.height==height;
    }

    int operator!=(const Resolution& right)const{
        return right.width!=width || right.height!=height;
    }

    int operator<(const Resolution& right)const{
        int result;
        if(width < right.width)
        	result|=1<<0;
        if(height < right.height)
        	result|=0x2;
        return result;
    }

    int operator<=(const Resolution& right)const{
        int result;
        if(width <= right.width)
        	result|= 1<<0;
        if(height <= right.height)
        	result|= 0x2;
        return result;
    }

    int operator>(const Resolution& right)const{
    	return ~operator<=(right);
    }

    int operator>=(const Resolution& right)const{
    	return ~operator<(right);
    }
};

}
