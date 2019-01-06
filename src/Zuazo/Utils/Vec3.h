#pragma once

#include "Vec2.h"

namespace Zuazo{

struct Vec3 : Vec2{
	double	z;

	Vec3() : Vec2(){
		z=0;
	}

	Vec3(double x, double y, double z) : Vec2(x, y){
		this->z=z;
	}

	Vec3(const Vec2& vec, double z=0) : Vec2(vec){
		this->z=z;
	}

	Vec3(const Vec3& vec) : Vec2(vec){
		this->z=vec.z;
	}

	operator bool() const{
		return Vec2::operator bool() || z;
	}

	operator double() const{
		return sqrt(x*x + y*y + z*z);
	}

    Vec3 operator*(const Vec3& right)const{
    	return Vec3(x*right.x, y*right.y, z*right.z);
    }

    static double crossProduct(const Vec3& vec1, const Vec3& vec2){
    	return vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z;
    }
};

}
