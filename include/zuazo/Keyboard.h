#pragma once

#include "Macros.h"
#include "Utils/Bit.h"

#include <string_view>

namespace Zuazo {

enum class KeyboardKey {
	NONE 			= -1,

	ASCII_BEGIN 	= 0x000,
	space 			= ' ',
	apostrophe 		= '\'',
	comma 			= ',',
	minus 			= '-',
	period 			= '.',
	slash 			= '/',
	nb0 			= '0',
	nb1 			= '1',
	nb2 			= '2',
	nb3 			= '3',
	nb4 			= '4',
	nb5 			= '5',
	nb6 			= '6',
	nb7 			= '7',
	nb8 			= '8',
	nb9 			= '9',
	semicolon 		= ';',
	equal 			= '=',
	a 				= 'A',
	b 				= 'B',
	c 				= 'C',
	d 				= 'D',
	e 				= 'E',
	f 				= 'F',
	g 				= 'G',
	h 				= 'H',
	i 				= 'I',
	j 				= 'J',
	k 				= 'K',
	l 				= 'L',
	m 				= 'M',
	n 				= 'N',
	o 				= 'O',
	p 				= 'P',
	q 				= 'Q',
	r 				= 'R',
	s 				= 'S',
	t 				= 'T',
	u 				= 'U',
	v 				= 'V',
	w 				= 'W',
	x 				= 'X',
	y 				= 'Y',
	z 				= 'Z',
	leftBracket 	= '[',
	backSlash 		= '\\',
	rightBracket 	= ']',
	graveAccent 	= '`',
	ASCII_END		= 0x100,

	NAV_BEGIN 		= ASCII_END,
	escape,
	enter,
	tab,
	backspace,
	insert,
	del,
	right,
	left,
	down,
	up,
	pageUp,
	pageDown,
	home,
	end,
	NAV_END 		= 0x200,

	LOCK_BEGIN		= NAV_END,
	capsLock,
	scrollLock,
	numLock,
	printScreen,
	pause,
	LOCK_END		= 0x300,

	FUNCTION_BEGIN	= LOCK_END,
	f1,
	f2,
	f3,
	f4,
	f5,
	f6,
	f7,
	f8,
	f9,
	f10,
	f11,
	f12,
	f13,
	f14,
	f15,
	f16,
	f17,
	f18,
	f19,
	f20,
	f21,
	f22,
	f23,
	f24,
	f25,
	FUNCTION_END	= 0x400,
	
	KEYPAD_BEGIN	= FUNCTION_END,
	kp0,
	kp1,
	kp2,
	kp3,
	kp4,
	kp5,
	kp6,
	kp7,
	kp8,
	kp9,
	kpDecimal,
	kpDivide,
	kpMultiply,
	kpSubstract,
	kpAdd,
	kpEnter,
	kpEqual,
	KEYPAD_END		= 0x500,

	MODIFIER_BEGIN	= KEYPAD_END,
	leftShift,
	leftControl,
	leftAlt,
	leftSuper,
	rightShift,
	rightControl,
	rightAlt,
	rightSuper,
	menu,
	MODIFIER_END	= 0x600,
};

enum class MouseKey {
	nb1,
	nb2,
	nb3,
	nb4,
	nb5,
	nb6,
	nb7,
	nb8,

	left 			= nb1,
	right			= nb2,
	middle			= nb3
};

enum class KeyModifiers {
	none			= 0,
	shift 			= Utils::bit(0),
	control			= Utils::bit(1),
	alt				= Utils::bit(2),
	super			= Utils::bit(3),
	capsLock		= Utils::bit(4),
	numLock			= Utils::bit(5)
};

enum class KeyEvent {
	release,
	press,
	repeat
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