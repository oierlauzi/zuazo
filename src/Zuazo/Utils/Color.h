#pragma once

#include <boost/algorithm/string/erase.hpp>
#include <stddef.h>
#include <sys/types.h>
#include <iostream>
#include <sstream>
#include <string>

namespace Zuazo{

struct Color{
	float	r;
	float	g;
	float	b;
	float	a;

	Color(){
		r=1.0;
		g=1.0;
		b=1.0;
		a=1.0;
	}

	Color(float red, float green, float blue, float alpha=1.0){
		r=red;
		g=green;
		b=blue;
		a=alpha;
	}

	Color(const Color& color, float alpha){
		r=color.r;
		g=color.g;
		b=color.b;
		a=alpha;
	}

	Color(const Color& color)=default;

	Color(u_int32_t rgba){
		setColorRgba(rgba);
	}

	Color(std::string str){
		boost::erase_all(str, "#");
		boost::erase_all(str, "0x");

		//Try to parse a number from string
		u_int32_t rgba;
		std::stringstream ss;
		ss << std::hex << str;
		ss >> rgba;

		size_t length=str.length();
		if(length==8){
			//Contains 8 characters -> Includes alpha
			setColorRgba(rgba);
		}else if(length==6){
			//Contains 6 characters -> Does not include alpha
			setColorRgba(rgba<<8);
			a=1.0;
		}else{
			//Not a valid string
			setColorRgba(0);
		}
	}

	/*
	 * Default colors
	 */

	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color CYAN;
	static const Color PURPLE;
	static const Color YELLOW;
	static const Color ORANGE;
	static const Color WHITE;
	static const Color GRAY;
	static const Color BLACK;

private:
	void setColorRgba(u_int32_t rgba);
};

inline void Color::setColorRgba(u_int32_t rgba){
	r=(float)((rgba >> 8*3) % 0x100) / 0xff;
	g=(float)((rgba >> 8*2) % 0x100) / 0xff;
	b=(float)((rgba >> 8*1) % 0x100) / 0xff;
	a=(float)((rgba >> 8*0) % 0x100) / 0xff;
}

const Color Color::RED=		Color(0xff0000ff);
const Color Color::GREEN=	Color(0x00ff00ff);
const Color Color::BLUE=	Color(0x0000ffff);
const Color Color::CYAN=	Color(0x00ffffff);
const Color Color::PURPLE=	Color(0xff00ffff);
const Color Color::YELLOW=	Color(0xffff00ff);
const Color Color::ORANGE=	Color(0xff4500ff);
const Color Color::WHITE=	Color(0xffffffff);
const Color Color::GRAY=	Color(0x7f7f7fff);
const Color Color::BLACK=	Color(0x000000ff);

}
