#pragma once

#include <GL/glew.h>
#include <stddef.h>
#include <sys/types.h>
#include <mutex>

#include "Utils/Resolution.h"
#include "Utils/ImgBuffer.h"

namespace Zuazo{

class Surface;
class Window;

class Image {
	friend Surface;
	friend Window;
public:
	Image();
	Image(const Image& image);
	Image(const ImgBuffer& extImage);
	Image(const Surface& surface);
	~Image();

	void						copy(const Image& image);
	void						copy(const ImgBuffer& extImage);
	void						copy(const Surface& surface);
	void						read(ImgBuffer * img);
private:
	Resolution					m_res;
	size_t						m_size;
	GLuint						m_pbo;

	std::mutex					m_mutex;

	void						resize(const Resolution& res);
	void						resize(u_int32_t width, u_int32_t height);
};

}
