#pragma once

#include <GL/glew.h>
#include <stddef.h>
#include <sys/types.h>
#include <mutex>

#include "Primitives.h"

namespace Zuazo{

class Surface;

class Image {
	friend Surface;
public:
	Image();
	Image(const Image& image);
	Image(const ExtImage& extImage);
	Image(const Surface& surface);
	~Image();

	void						copy(const Image& image);
	void						copy(const ExtImage& extImage);
	void						copy(const Surface& surface);
private:
	Resolution					m_res;
	size_t						m_size;
	GLuint						m_pbo;

	std::mutex					m_mutex;

	void						resize(const Resolution& res);
	void						resize(u_int32_t width, u_int32_t height);
};

}
