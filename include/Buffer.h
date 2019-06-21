#pragma once

#include <stddef.h>
#include <stdlib.h>
#include <cstring>

namespace Zuazo {

class Buffer{
public:
	Buffer();
	Buffer(size_t size);
	Buffer(size_t size, const void* data);
	Buffer(const Buffer& other);
	Buffer(Buffer&& other);
	~Buffer();

	void			reallocate(size_t size);

	void*			get();
	const void*		get() const;
	size_t			getSize() const;
private:
	void*			m_data;
	size_t			m_size;
};

}

#include "Buffer.inl"
