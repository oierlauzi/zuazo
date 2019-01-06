#pragma once

#include <sys/types.h>

namespace Zuazo{

struct Resolution{
	u_int32_t	width=0;
	u_int32_t	height=0;

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
