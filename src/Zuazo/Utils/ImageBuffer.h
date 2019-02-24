#pragma once

#include <stdlib.h>
#include <sys/types.h>

#include "ImageAttributes.h"

namespace Zuazo::Utils{

struct ImageBuffer{
	ImageAttributes		att;
	u_int8_t *			data;

	ImageBuffer() :
		data(nullptr)
	{
	}

	ImageBuffer(const ImageAttributes& att) :
		att(att)
	{
		data=(u_int8_t*) malloc(att.size());
	}

	~ImageBuffer(){
		if(data)
			free(data);
	}
};

}
