#pragma once

#include <sys/types.h>

namespace Zuazo{

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

	operator bool() const{
		return width && height;
	}

    int operator==(const Resolution& right)const{
        return right.width==width && right.height==height;
    }

    int operator!=(const Resolution& right)const{
        return right.width!=width || right.height!=height;
    }
};

}
