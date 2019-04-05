#pragma once

#include "../Utils/Vector.h"

namespace Zuazo::Graphics{

typedef float VectorComponent;

struct Rectangle{
	Utils::Vec<VectorComponent, 3>	center;
	Utils::Vec<VectorComponent, 2>	size;

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
	Utils::Vec<VectorComponent, 3>	bottomLeft;
	Utils::Vec<VectorComponent, 3>	topLeft;
	Utils::Vec<VectorComponent, 3>	bottomRight;
	Utils::Vec<VectorComponent, 3>	topRight;

	Quad()=default;
	Quad(const Quad& other)=default;
	Quad(const Rectangle& rect);
	~Quad()=default;

	const Utils::Vec<VectorComponent, 3>* toVertexBuffer() const;
};

inline Quad::Quad(const Rectangle& rect) :
	bottomLeft(	-rect.center.x,					-rect.center.y,					-rect.center.z),
	topLeft(	-rect.center.x,					-rect.center.y + rect.size.y,	-rect.center.z),
	bottomRight(-rect.center.x + rect.size.x,	-rect.center.y, 				-rect.center.z),
	topRight(	-rect.center.x + rect.size.x,	-rect.center.y + rect.size.y,	-rect.center.z)
{
}

inline const Utils::Vec<VectorComponent, 3>* Quad::toVertexBuffer() const{
	return reinterpret_cast<const Utils::Vec<VectorComponent, 3>*>(this);
}

}
