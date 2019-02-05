#pragma once

#include <GL/glew.h>

#include "../../Utils/Resolution.h"
#include "PixelIO.h"

namespace Zuazo::Graphics::GL{
class FrameBuffer;

class Texture2D{
	friend FrameBuffer;
public:
	Texture2D();
	Texture2D(const Texture2D& other);
	~Texture2D();

	void						setRes(const Utils::Resolution& res);
	const Utils::Resolution&	getRes();

	void						subImage(const PixelBuffer& buf);
	void						subImage(const PixelUnpackBuffer& buf);
private:
	GLuint						m_tex;
	Utils::Resolution			m_resolution;
};
}
