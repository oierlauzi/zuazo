#pragma once

#include <GL/glew.h>
#include <stddef.h>
#include <sys/types.h>
#include <string>

#include "../Primitives.h"
#include "../Shader.h"

namespace Zuazo::Shapes {

class Shape {
public:
	static 	int		init();
	static 	int		end();

	Shape(u_int32_t nVert);
	Shape(const Shape& shape);
	virtual ~Shape();

	virtual void					drawTexture() const;
	virtual void					drawPerimeter() const;
	virtual void					drawSolid() const;
protected:

	//Constants
	static const u_int32_t 			VERT_STRIDE=3;
	static const u_int32_t 			TEX_STRIDE=2;

	static const std::string 		TEXTURE_VERT_SHADER;
	static const std::string 		TEXTURE_FRAG_SHADER;
	static const std::string 		LINE_VERT_SHADER;
	static const std::string 		LINE_FRAG_SHADER;
	static const std::string 		SOLID_VERT_SHADER;
	static const std::string 		SOLID_FRAG_SHADER;
	static const std::string 		ALPHA_VERT_SHADER;
	static const std::string 		ALPHA_FRAG_SHADER;

	u_int32_t						m_nVert;
	GLuint							m_vao;
	GLuint							m_vbos[2];

	float *							m_loadedVertices;
	float *							m_loadedTexCoords;
	size_t							m_vertSize;
	size_t							m_texCoordSize;

	static Shader					s_texShader;
	static Shader					s_lineShader;
	static Shader					s_solidShader;
	static Shader					s_alphaShader;

	void							setData(const Vec3* vertices, const Vec2* textureCoords);

};

}
