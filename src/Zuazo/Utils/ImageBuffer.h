#pragma once

#include <sys/types.h>

#include "ImageAttributes.h"

namespace Zuazo::Utils{

struct ImageBuffer{
	ImageAttributes		att;
	u_int8_t *			data;
};

}
