#pragma once

#include <GL/glew.h>
#include <stddef.h>
#include <sys/types.h>
#include <mutex>

#include "Primitives.h"

namespace Zuazo{

class Surface;

class Uploader {
	friend Surface;
public:
	Uploader();
	~Uploader();

	void						upload(ExtImage& frame);
private:
	//Class constants
	static const u_int32_t 		BUFFERS=2;

	struct Buffer{
		Resolution					res;
		size_t						size;
		GLuint						pbo=0;
	};

	Buffer						m_buffers[BUFFERS];
	u_int32_t					m_currBuffer;

	std::mutex					m_mutex;
};

}
