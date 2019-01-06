#pragma once

#include <stddef.h>
#include <sys/types.h>

#include "Resolution.h"

namespace Zuazo{

/*
 * It is used to exchange images with Zuazo
 */
struct ImgBuffer{
	Resolution res;
	u_int8_t* data=NULL;

	inline size_t getSize(){
		return res.width * res.height * 4;
	}
};

}
