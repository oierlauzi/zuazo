#pragma once

#include <GL/glew.h>
#include <sys/types.h>
#include <condition_variable>
#include <mutex>
#include <set>
#include <vector>
#include <thread>

#include "Frame.h"
#include "Context.h"
#include "Primitives.h"


namespace Zuazo {

class Uploader {
public:
	Uploader();
	~Uploader();

	void						upload(MemFrame& frame);
private:
	//Class constants
	static const u_int32_t 		BUFFERS=2;

	struct Buffer{
		u_int32_t					width=0;
		u_int32_t					height=0;
        u_int64_t              		size=0;

		GLuint						pbo=0;
	};

	Buffer						m_buffers[BUFFERS];
	u_int32_t					m_currBuffer;

	std::mutex					m_mutex;

	static Context&				getContext();
	static void					unuseContext(Context& ctx);
};

}

