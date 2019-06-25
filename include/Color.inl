#define RGBA_COLOR_COMP(color, x) ((color >> 8*x) & 0xff)
#define NORMALIZE_BYTE(x) (static_cast<float>(x) / 0xffU)

namespace Zuazo {

constexpr Color::Color() :
	r(1.0),
	g(1.0),
	b(1.0),
	a(1.0)
{
}

constexpr Color::Color(float red, float green, float blue, float alpha) :
	r(red),
	g(green),
	b(blue),
	a(alpha)
{
}

constexpr Color::Color(u_int8_t red, u_int8_t green, u_int8_t blue, u_int8_t alpha) :
	r(NORMALIZE_BYTE(red)),
	g(NORMALIZE_BYTE(green)),
	b(NORMALIZE_BYTE(blue)),
	a(NORMALIZE_BYTE(alpha))
{
}

constexpr Color::Color(const Color& color, float alpha):
	r(color.r),
	g(color.g),
	b(color.b),
	a(alpha)
{
}

constexpr Color::Color(u_int32_t rgba) : 
	Color(
		static_cast<u_int8_t>(RGBA_COLOR_COMP(rgba, 0)),
		static_cast<u_int8_t>(RGBA_COLOR_COMP(rgba, 1)),
		static_cast<u_int8_t>(RGBA_COLOR_COMP(rgba, 2)),
		static_cast<u_int8_t>(RGBA_COLOR_COMP(rgba, 3))
	)
{
}

inline Color::Color(const std::string& str) : 
	Color(parseColor(str))
{
}

inline u_int32_t Color::parseColor(std::string str){
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

}