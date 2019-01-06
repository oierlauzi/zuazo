#pragma once

#include "Vec3.h"

namespace Zuazo{

struct Vec4 : Vec3{
	double	w;

	Vec4() : Vec3(){
		w=0;
	}

	Vec4(double x, double y, double z, double w) : Vec3(x, y, z){
		this->w=w;
	}

	Vec4(const Vec2& vec, double z=0, double w=0) : Vec3(vec, z){
		this->w=w;
	}

	Vec4(const Vec3& vec, double w=0) : Vec3(vec){
		this->w=w;
	}

	Vec4(const Vec4& vec) : Vec3(vec){
		this->w=vec.w;
	}

	operator bool() const{
		return Vec3::operator bool() || w;
	}

	operator double() const{
		return sqrt(x*x + y*y + z*z + w*w);
	}

    Vec4 operator*(const Vec4& right)const{
    	return Vec4(x*right.x, y*right.y, z*right.z, w*right.w);
    }

    static double crossProduct(const Vec4& vec1, const Vec4& vec2){
    	return vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z + vec1.w*vec2.w;
    }
};

}
