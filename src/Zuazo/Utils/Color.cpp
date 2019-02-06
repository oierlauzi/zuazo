#include "Color.h"

using namespace Zuazo::Utils;
/*
 * STATIC VARIABLES
 */

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

/*
 * METHODS
 */

u_int32_t Color::parseColor(std::string str){
	boost::erase_all(str, "#");
	boost::erase_all(str, "0x");

	//Try to parse a number from string
	u_int32_t rgba;
	std::stringstream ss; //A bit nazi
	ss << std::hex << str;
	ss >> rgba;

	size_t length=str.length();
	if(length==8){
		//Contains 8 characters -> Includes alpha
		//Nothing to change
	}else if(length==6){
		//Contains 6 characters -> Does not include alpha
		rgba<<=8; //Shift bits to make room for alpha
		rgba+=0xff; //Add alpha
	}else{
		//Not a valid string
		rgba=0; //Simply set it black and zero opacity
	}

	return rgba;
}
