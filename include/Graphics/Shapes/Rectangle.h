#pragma once

#include <array>

#include "Quads.h"
#include "Shape.h"
#include "../../Utils/Vector.h"

namespace Zuazo::Graphics::Shapes{

class Rectangle : public Quads<2>{
public:
	struct RectangleData{
		Utils::Vec<2, float> 	pos;
		Utils::Vec<2, float> 	size;
	};

	struct RectangleVertices{
		Utils::Vec<2, float> 	v0;
		Utils::Vec<2, float> 	v1;
	};

	struct RectangleTexCoords{
		Utils::Vec<2, float> 	c0;
		Utils::Vec<2, float> 	c1;
	};

	Rectangle()=default;

	Rectangle(const RectangleData& data);
	Rectangle(const RectangleVertices& vertices);
	Rectangle(const RectangleData& data, const RectangleTexCoords& texCoords);
	Rectangle(const RectangleVertices& vertices, const RectangleTexCoords& texCoords);

	Rectangle(const Rectangle& other)=delete; //TODO
	Rectangle(Rectangle&& other)=default; //TODO
	virtual ~Rectangle()=default;

	void 							upload(const RectangleData& data);
	void 							upload(const RectangleVertices& vertices);
	void 							upload(const RectangleTexCoords& texCoords);
	void 							upload(const RectangleData& data, const RectangleTexCoords& texCoords);
	void 							upload(const RectangleVertices& vertices, const RectangleTexCoords& texCoords);
private:
	static Quads<2>::Vertices		getVertices(const RectangleData& data);
	static Quads<2>::Vertices 		getVertices(const RectangleVertices& vertices);
	static Quads<2>::TexCoords		getTexCoords(const RectangleTexCoords& data);
};


inline Rectangle::Rectangle(const RectangleData& data) :
	Quads<2>(getVertices(data))
{
}


inline Rectangle::Rectangle(const RectangleVertices& vertices) :
	Quads<2>(getVertices(vertices))
{
}


inline Rectangle::Rectangle(const RectangleData& data, const RectangleTexCoords& texCoords) :
	Quads<2>(getVertices(data), getTexCoords(texCoords))
{
}


inline Rectangle::Rectangle(const RectangleVertices& vertices, const RectangleTexCoords& texCoords) :
	Quads<2>(getVertices(vertices), getTexCoords(texCoords))
{
}

inline void Rectangle::upload(const RectangleData& data){
	Quads::uploadVertices(getVertices(data));
}


inline void Rectangle::upload(const RectangleVertices& vertices){
	Quads::uploadVertices(getVertices(vertices));
}

inline void Rectangle::upload(const RectangleTexCoords& texCoords){
	Quads::uploadTexCoords(getTexCoords(texCoords));
}

inline void Rectangle::upload(const RectangleData& data, const RectangleTexCoords& texCoords){
	Quads::uploadVertices(getVertices(data));
	Quads::uploadTexCoords(getTexCoords(texCoords));
}


inline void Rectangle::upload(const RectangleVertices& vertices, const RectangleTexCoords& texCoords){
	Quads::uploadVertices(getVertices(vertices));
	Quads::uploadTexCoords(getTexCoords(texCoords));
}


inline Quads<2>::Vertices	Rectangle::getVertices(const RectangleData& data){
	return Quads<2>::Vertices
	{
		Utils::Vec<2, float>{data.pos.x,				data.pos.y					},
		Utils::Vec<2, float>{data.pos.x,				data.pos.y + data.size.y	},
		Utils::Vec<2, float>{data.pos.x + data.size.x,	data.pos.y 					},
		Utils::Vec<2, float>{data.pos.x + data.size.x,	data.pos.y + data.size.y	}
	};
}


inline Quads<2>::Vertices Rectangle::getVertices(const RectangleVertices& vert){
	return Quads<2>::Vertices
	{
		Utils::Vec<2, float>{vert.v0.x,					vert.v0.y					},
		Utils::Vec<2, float>{vert.v0.x,					vert.v1.y					},
		Utils::Vec<2, float>{vert.v1.x,					vert.v0.y					},
		Utils::Vec<2, float>{vert.v1.x,					vert.v1.y					}
	};
}


inline Quads<2>::TexCoords Rectangle::getTexCoords(const RectangleTexCoords& tc){
	return Quads<2>::Vertices
	{
		Utils::Vec<2, float>{tc.c0.x,					tc.c0.y						},
		Utils::Vec<2, float>{tc.c0.x,					tc.c1.y						},
		Utils::Vec<2, float>{tc.c1.x,					tc.c0.y						},
		Utils::Vec<2, float>{tc.c1.x,					tc.c1.y						}
	};
}
}
