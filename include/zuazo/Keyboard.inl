#include "Keyboard.h"

#include "StringConversions.h"

namespace Zuazo {

constexpr std::string_view toString(KeyboardKey key) noexcept {
	switch(key){

	case KeyboardKey::space:			return " ";
	case KeyboardKey::apostrophe:		return "\'";
	case KeyboardKey::comma:			return ",";
	case KeyboardKey::minus:			return "-";
	case KeyboardKey::period:			return ".";
	case KeyboardKey::slash:			return "/";
	case KeyboardKey::nb0:				return "0";
	case KeyboardKey::nb1:				return "1";
	case KeyboardKey::nb2:				return "2";
	case KeyboardKey::nb3:				return "3";
	case KeyboardKey::nb4:				return "4";
	case KeyboardKey::nb5:				return "5";
	case KeyboardKey::nb6:				return "6";
	case KeyboardKey::nb7:				return "7";
	case KeyboardKey::nb8:				return "8";
	case KeyboardKey::nb9:				return "9";
	case KeyboardKey::semicolon:		return ";";
	case KeyboardKey::equal:			return "=";
	case KeyboardKey::a:				return "A";
	case KeyboardKey::b:				return "B";
	case KeyboardKey::c:				return "C";
	case KeyboardKey::d:				return "D";
	case KeyboardKey::e:				return "E";
	case KeyboardKey::f:				return "F";
	case KeyboardKey::g:				return "G";
	case KeyboardKey::h:				return "H";
	case KeyboardKey::i:				return "I";
	case KeyboardKey::j:				return "J";
	case KeyboardKey::k:				return "K";
	case KeyboardKey::l:				return "L";
	case KeyboardKey::m:				return "M";
	case KeyboardKey::n:				return "N";
	case KeyboardKey::o:				return "O";
	case KeyboardKey::p:				return "P";
	case KeyboardKey::q:				return "Q";
	case KeyboardKey::r:				return "R";
	case KeyboardKey::s:				return "S";
	case KeyboardKey::t:				return "T";
	case KeyboardKey::u:				return "U";
	case KeyboardKey::v:				return "V";
	case KeyboardKey::w:				return "W";
	case KeyboardKey::x:				return "X";
	case KeyboardKey::y:				return "Y";
	case KeyboardKey::z:				return "Z";
	case KeyboardKey::leftBracket:		return "[";
	case KeyboardKey::backSlash:		return "\\";
	case KeyboardKey::rightBracket:		return "]";
	case KeyboardKey::graveAccent:		return "`";

	ZUAZO_ENUM2STR_CASE( KeyboardKey, escape)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, enter)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, tab)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, backspace)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, insert)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, del)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, right)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, left)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, down)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, up)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, pageUp)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, pageDown)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, home)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, end)

	ZUAZO_ENUM2STR_CASE( KeyboardKey, capsLock)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, scrollLock)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, numLock)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, printScreen)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, pause)

	ZUAZO_ENUM2STR_CASE( KeyboardKey, f1)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f2)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f3)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f4)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f5)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f6)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f7)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f8)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f9)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f10)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f11)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f12)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f13)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f14)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f15)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f16)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f17)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f18)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f19)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f20)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f21)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f22)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f23)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f24)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, f25)

	ZUAZO_ENUM2STR_CASE( KeyboardKey, kp0)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kp1)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kp2)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kp3)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kp4)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kp5)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kp6)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kp7)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kp8)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kp9)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kpDecimal)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kpDivide)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kpMultiply)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kpSubstract)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kpAdd)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kpEnter)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, kpEqual)

	ZUAZO_ENUM2STR_CASE( KeyboardKey, leftShift)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, leftControl)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, leftAlt)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, leftSuper)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, rightShift)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, rightControl)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, rightAlt)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, rightSuper)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, menu)

	default: return "";
	}
}

constexpr std::string_view toString(MouseKey key) noexcept {
	switch(key){

	ZUAZO_ENUM2STR_CASE( MouseKey, left)
	ZUAZO_ENUM2STR_CASE( MouseKey, right)
	ZUAZO_ENUM2STR_CASE( MouseKey, middle)
	ZUAZO_ENUM2STR_CASE( MouseKey, nb4)
	ZUAZO_ENUM2STR_CASE( MouseKey, nb5)
	ZUAZO_ENUM2STR_CASE( MouseKey, nb6)
	ZUAZO_ENUM2STR_CASE( MouseKey, nb7)
	ZUAZO_ENUM2STR_CASE( MouseKey, nb8)

	default: return "";
	}
}

constexpr std::string_view toString(KeyEvent event) noexcept {
	switch(event){

	ZUAZO_ENUM2STR_CASE( KeyEvent, release)
	ZUAZO_ENUM2STR_CASE( KeyEvent, press)
	ZUAZO_ENUM2STR_CASE( KeyEvent, repeat)

	default: return "";
	}
}


inline std::ostream& operator<<(std::ostream& os, KeyboardKey key) {
	return os << toString(key);
}

inline std::ostream& operator<<(std::ostream& os, MouseKey key) {
	return os << toString(key);
}

inline std::ostream& operator<<(std::ostream& os, KeyEvent evnt) {
	return os << toString(evnt);
}


}