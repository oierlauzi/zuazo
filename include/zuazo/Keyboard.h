#pragma once

#include "Macros.h"
#include "Utils/Bit.h"

#include <string_view>

namespace Zuazo {

enum class KeyboardKey {
	NONE 			= -1,

	ASCII_BEGIN 	= 0x000,
	SPACE 			= ' ',
	APOSTROPHE 		= '\'',
	COMMA 			= ',',
	MINUS 			= '-',
	PERIOD 			= '.',
	SLASH 			= '/',
	NB0 			= '0',
	NB1 			= '1',
	NB2 			= '2',
	NB3 			= '3',
	NB4 			= '4',
	NB5 			= '5',
	NB6 			= '6',
	NB7 			= '7',
	NB8 			= '8',
	NB9 			= '9',
	SEMICOLON 		= ';',
	EQUAL 			= '=',
	A 				= 'A',
	B 				= 'B',
	C 				= 'C',
	D 				= 'D',
	E 				= 'E',
	F 				= 'F',
	G 				= 'G',
	H 				= 'H',
	I 				= 'I',
	J 				= 'J',
	K 				= 'K',
	L 				= 'L',
	M 				= 'M',
	N 				= 'N',
	O 				= 'O',
	P 				= 'P',
	Q 				= 'Q',
	R 				= 'R',
	S 				= 'S',
	T 				= 'T',
	U 				= 'U',
	V 				= 'V',
	W 				= 'W',
	X 				= 'X',
	Y 				= 'Y',
	Z 				= 'Z',
	LEFT_BRACKET 	= '[',
	BACKSLASH 		= '\\',
	RIGHT_BRACKET 	= ']',
	GRAVE_ACCENT 	= '`',
	ASCII_END		= 0x100,

	NAV_BEGIN 		= ASCII_END,
	ESCAPE,
	ENTER,
	TAB,
	BACKSPACE,
	INSERT,
	DELETE,
	RIGHT,
	LEFT,
	DOWN,
	UP,
	PAGE_UP,
	PAGE_DOWN,
	HOME,
	END,
	NAV_END 		= 0x200,

	LOCK_BEGIN		= NAV_END,
	CAPS_LOCK,
	SCROLL_LOCK,
	NUM_LOCK,
	PRINT_SCREEN,
	PAUSE,
	LOCK_END		= 0x300,

	FUNCTION_BEGIN	= LOCK_END,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	F13,
	F14,
	F15,
	F16,
	F17,
	F18,
	F19,
	F20,
	F21,
	F22,
	F23,
	F24,
	F25,
	FUNCTION_END	= 0x400,
	
	KEYPAD_BEGIN	= FUNCTION_END,
	KP_0,
	KP_1,
	KP_2,
	KP_3,
	KP_4,
	KP_5,
	KP_6,
	KP_7,
	KP_8,
	KP_9,
	KP_DECIMAL,
	KP_DIVIDE,
	KP_MULTIPLY,
	KP_SUBTRACT,
	KP_ADD,
	KP_ENTER,
	KP_EQUAL,
	KEYPAD_END		= 0x500,

	MODIFIER_BEGIN	= KEYPAD_END,
	LEFT_SHIFT,
	LEFT_CONTROL,
	LEFT_ALT,
	LEFT_SUPER,
	RIGHT_SHIFT,
	RIGHT_CONTROL,
	RIGHT_ALT,
	RIGHT_SUPER,
	MENU,
	MODIFIER_END	= 0x600,
};

enum class MouseKey {
	NB1,
	NB2,
	NB3,
	NB4,
	NB5,
	NB6,
	NB7,
	NB8,

	LEFT 			= NB1,
	RIGHT			= NB2,
	MIDDLE			= NB3
};

enum class KeyModifiers {
	NONE			= 0,
	SHIFT 			= Utils::bit(0),
	CONTROL			= Utils::bit(1),
	ALT				= Utils::bit(2),
	SUPER			= Utils::bit(3),
	CAPS_LOCK		= Utils::bit(4),
	NUM_LOCK		= Utils::bit(5)
};

enum class KeyEvent {
	RELEASE,
	PRESS,
	REPEAT
};

ZUAZO_ENUM_COMP_OPERATORS(KeyboardKey)
ZUAZO_ENUM_COMP_OPERATORS(MouseKey)

constexpr std::string_view toString(KeyboardKey key) noexcept;
constexpr std::string_view toString(MouseKey key) noexcept;
constexpr std::string_view toString(KeyEvent event) noexcept;

std::ostream& operator<<(std::ostream& os, KeyboardKey key);
std::ostream& operator<<(std::ostream& os, MouseKey key);
std::ostream& operator<<(std::ostream& os, KeyEvent evnt);

}

#include "Keyboard.inl"