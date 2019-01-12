#pragma once

#include <cmath>

namespace Zuazo{

struct Vec2{
	double	x;
	double	y;

	Vec2(){
		x=0;
		y=0;
	}

	Vec2(double x, double y){
		this->x=x;
		this->y=y;
	}

	Vec2(const Vec2& vec){
		x=vec.x;
		y=vec.y;
	}

	operator bool() const{
		return x || y;
	}

	operator double() const{
		return sqrt(x*x + y*y);
	}

    Vec2 operator*(const Vec2& right)const{
    	return Vec2(x*right.x, y*right.y);
    }

    Vec2 operator*(double right)const{
    	return Vec2(x*right, y*right);
    }

    static double crossProduct(const Vec2& vec1, const Vec2& vec2){
    	return vec1.x*vec2.x + vec1.y*vec2.y;
    }
};

}
