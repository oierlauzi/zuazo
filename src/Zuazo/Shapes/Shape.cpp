#include "Shape.h"

#include "../Context.h"

using namespace Zuazo::Shapes;

/********************************
 *		SHADER DEFINITIONS		*
 ********************************/

const std::string Shape::TEXTURE_VERT_SHADER=
		#include "../../../shaders/texture.vert"
		;
const std::string Shape::TEXTURE_FRAG_SHADER=
		#include "../../../shaders/texture.frag"
		;
const std::string Shape::LINE_VERT_SHADER=
		#include "../../../shaders/line.vert"
		;
const std::string Shape::LINE_FRAG_SHADER=
		#include "../../../shaders/line.frag"
		;
const std::string Shape::SOLID_VERT_SHADER=
		#include "../../../shaders/solid.vert"
		;
const std::string Shape::SOLID_FRAG_SHADER=
		#include "../../../shaders/solid.frag"
		;
const std::string Shape::ALPHA_VERT_SHADER;
const std::string Shape::ALPHA_FRAG_SHADER;


Zuazo::Shader Shape::s_texShader;
Zuazo::Shader Shape::s_lineShader;
Zuazo::Shader Shape::s_solidShader;
Zuazo::Shader Shape::s_alphaShader;

int Shape::init() {
	s_texShader=Shader(TEXTURE_VERT_SHADER, TEXTURE_FRAG_SHADER);
	s_lineShader=Shader(LINE_VERT_SHADER, LINE_FRAG_SHADER);
	s_solidShader=Shader(SOLID_VERT_SHADER, SOLID_FRAG_SHADER);
	s_alphaShader=Shader(ALPHA_VERT_SHADER, ALPHA_FRAG_SHADER);
	return 0;
}

int Shape::end() {
	return 0;
}

/********************************
 *	CONSTRUCTOR / DESTRUCTOR	*
 ********************************/


Shape::Shape(u_int32_t nVert) {
}

Shape::Shape(const Shape& shape) : Shape(shape.m_nVert){
}

Shape::~Shape() {
}

/********************************
 *		DRAW ACTIONS			*
 ********************************/

inline void Shape::drawTexture() const {
}

inline void Shape::drawPerimeter() const {
}

inline void Shape::drawSolid() const {
}


/********************************
 *		PRIVATE METHODS			*
 ********************************/

void Shape::setData(const Vec3* vertices, const Vec2* textureCoords) {
}
