#pragma once

#include "../Utils/Vector.h"

namespace Zuazo::Graphics{

typedef float VectorComponent;

struct Rectangle{
	Utils::Vec<3, VectorComponent>	center;
	Utils::Vec<2, VectorComponent>	size;

	Rectangle()=default;
	Rectangle(const Rectangle& other)=default;
	~Rectangle()=default;

	int operator==(const Rectangle& other);
	int operator!=(const Rectangle& other);
};

inline int Rectangle::operator==(const Rectangle& other){
	return center == other.center && size == other.size;
}

inline int Rectangle::operator!=(const Rectangle& other){
	return center != other.center || size != other.size;
}

struct Quad{
	Utils::Vec<3, VectorComponent>	bottomLeft;
	Utils::Vec<3, VectorComponent>	topLeft;
	Utils::Vec<3, VectorComponent>	bottomRight;
	Utils::Vec<3, VectorComponent>	topRight;

	Quad()=default;
	Quad(const Quad& other)=default;
	Quad(const Rectangle& rect);
	~Quad()=default;
};

inline Quad::Quad(const Rectangle& rect) :
	bottomLeft(	-rect.center.x,					-rect.center.y,					-rect.center.z),
	topLeft(	-rect.center.x,					-rect.center.y + rect.size.y,	-rect.center.z),
	bottomRight(-rect.center.x + rect.size.x,	-rect.center.y, 				-rect.center.z),
	topRight(	-rect.center.x + rect.size.x,	-rect.center.y + rect.size.y,	-rect.center.z)
{
}

}
