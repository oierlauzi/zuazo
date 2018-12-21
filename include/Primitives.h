#pragma once

#include <sys/types.h>

namespace Zuazo{

struct Vec2{
	double	x=0.0;
	double	y=0.0;
};

struct Vec3 : Vec2{
	double	z=0.0;
};

struct Vec4 : Vec3{
	double	w=0.0;
};


struct Color{
	float	r=1.0;
	float	g=1.0;
	float	b=1.0;
	float	a=1.0;
};

/*
 * A rectangle defined by the coordinates of
 * two opposing vertices
 */
struct Rectangle2C{
	Vec2	pos0;
	Vec2	pos1;
};

/*
 * A rectangle defined by the coordinate of its
 * top-left vertex and it's size
 */
struct RectangleCS{
	Vec2	pos;
	Vec2	size;
};

/*
 * Contains a resolution
 */
struct Resolution{
	u_int32_t	width=0;
	u_int32_t	height=0;
};

/*
 * Contains a rational number
 */
struct Rational{
	int num=0;
	int den=0;
};

}
