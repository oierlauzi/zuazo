#include "Keyboard.h"

namespace Zuazo {

constexpr std::string_view toString(KeyboardKey key) noexcept {
	switch(key){

	case KeyboardKey::SPACE:			return " ";
	case KeyboardKey::APOSTROPHE:		return "\'";
	case KeyboardKey::COMMA:			return ",";
	case KeyboardKey::MINUS:			return "-";
	case KeyboardKey::PERIOD:			return ".";
	case KeyboardKey::SLASH:			return "/";
	case KeyboardKey::NB0:				return "0";
	case KeyboardKey::NB1:				return "1";
	case KeyboardKey::NB2:				return "2";
	case KeyboardKey::NB3:				return "3";
	case KeyboardKey::NB4:				return "4";
	case KeyboardKey::NB5:				return "5";
	case KeyboardKey::NB6:				return "6";
	case KeyboardKey::NB7:				return "7";
	case KeyboardKey::NB8:				return "8";
	case KeyboardKey::NB9:				return "9";
	case KeyboardKey::SEMICOLON:		return ";";
	case KeyboardKey::EQUAL:			return "=";
	case KeyboardKey::A:				return "A";
	case KeyboardKey::B:				return "B";
	case KeyboardKey::C:				return "C";
	case KeyboardKey::D:				return "D";
	case KeyboardKey::E:				return "E";
	case KeyboardKey::F:				return "F";
	case KeyboardKey::G:				return "G";
	case KeyboardKey::H:				return "H";
	case KeyboardKey::I:				return "I";
	case KeyboardKey::J:				return "J";
	case KeyboardKey::K:				return "K";
	case KeyboardKey::L:				return "L";
	case KeyboardKey::M:				return "M";
	case KeyboardKey::N:				return "N";
	case KeyboardKey::O:				return "O";
	case KeyboardKey::P:				return "P";
	case KeyboardKey::Q:				return "Q";
	case KeyboardKey::R:				return "R";
	case KeyboardKey::S:				return "S";
	case KeyboardKey::T:				return "T";
	case KeyboardKey::U:				return "U";
	case KeyboardKey::V:				return "V";
	case KeyboardKey::W:				return "W";
	case KeyboardKey::X:				return "X";
	case KeyboardKey::Y:				return "Y";
	case KeyboardKey::Z:				return "Z";
	case KeyboardKey::LEFT_BRACKET:		return "[";
	case KeyboardKey::BACKSLASH:		return "\\";
	case KeyboardKey::RIGHT_BRACKET:	return "]";
	case KeyboardKey::GRAVE_ACCENT:		return "`";

	ZUAZO_ENUM2STR_CASE( KeyboardKey, ESCAPE)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, ENTER)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, TAB)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, BACKSPACE)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, INSERT)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, DELETE)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, RIGHT)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, LEFT)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, DOWN)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, UP)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, PAGE_UP)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, PAGE_DOWN)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, HOME)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, END)

	ZUAZO_ENUM2STR_CASE( KeyboardKey, CAPS_LOCK)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, SCROLL_LOCK)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, NUM_LOCK)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, PRINT_SCREEN)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, PAUSE)

	ZUAZO_ENUM2STR_CASE( KeyboardKey, F1)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F2)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F3)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F4)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F5)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F6)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F7)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F8)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F9)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F10)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F11)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F12)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F13)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F14)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F15)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F16)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F17)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F18)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F19)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F20)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F21)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F22)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F23)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F24)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, F25)

	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_0)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_1)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_2)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_3)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_4)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_5)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_6)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_7)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_8)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_9)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_DECIMAL)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_DIVIDE)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_MULTIPLY)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_SUBTRACT)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_ADD)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_ENTER)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, KP_EQUAL)

	ZUAZO_ENUM2STR_CASE( KeyboardKey, LEFT_SHIFT)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, LEFT_CONTROL)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, LEFT_ALT)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, LEFT_SUPER)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, RIGHT_SHIFT)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, RIGHT_CONTROL)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, RIGHT_ALT)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, RIGHT_SUPER)
	ZUAZO_ENUM2STR_CASE( KeyboardKey, MENU)

	default: return "";
	}
}

constexpr std::string_view toString(MouseKey key) noexcept {
	switch(key){

	case MouseKey::LEFT:	return "LEFT";
	case MouseKey::RIGHT:	return "RIGHT";
	case MouseKey::MIDDLE:	return "MIDDLE";

	ZUAZO_ENUM2STR_CASE( MouseKey, NB4)
	ZUAZO_ENUM2STR_CASE( MouseKey, NB5)
	ZUAZO_ENUM2STR_CASE( MouseKey, NB6)
	ZUAZO_ENUM2STR_CASE( MouseKey, NB7)
	ZUAZO_ENUM2STR_CASE( MouseKey, NB8)

	default: return "";
	}
}

constexpr std::string_view toString(KeyEvent evnt) noexcept {
	switch(evnt){

	ZUAZO_ENUM2STR_CASE( KeyEvent, RELEASE)
	ZUAZO_ENUM2STR_CASE( KeyEvent, PRESS)
	ZUAZO_ENUM2STR_CASE( KeyEvent, REPEAT)

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