#pragma once

#include "../Utils/Vector.h"

#include <array>

namespace Zuazo::Graphics{

struct Rectangle{
	Utils::Vec2f			size;

	Rectangle()=default;
	Rectangle(const Rectangle& other)=default;
	~Rectangle()=default;

	int operator==(const Rectangle& other);
	int operator!=(const Rectangle& other);
};

inline int Rectangle::operator==(const Rectangle& other){
	return size == other.size;
}

inline int Rectangle::operator!=(const Rectangle& other){
	return size != other.size;
}

struct Quad{
	std::array<Utils::Vec2f, 4>	vertices;

	Quad()=default;
	Quad(const Quad& other)=default;
	Quad(const Rectangle& rect);
	~Quad()=default;
};

inline Quad::Quad(const Rectangle& rect) :
	vertices{
		Utils::Vec2f(0.0,			0.0			),
		Utils::Vec2f(0.0,			rect.size.y	),
		Utils::Vec2f(rect.size.x,	0.0			),
		Utils::Vec2f(rect.size.x,	rect.size.y	)
	}
{
}
}
