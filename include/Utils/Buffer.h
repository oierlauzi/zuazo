#pragma once

#include "Hasher.h"

#include <stddef.h>
#include <stdlib.h>
#include <cstring>

namespace Zuazo::Utils {

class Buffer{
public:
	constexpr Buffer();
	Buffer(size_t size);
	Buffer(size_t size, const void* data);
	Buffer(const Buffer& other);
	Buffer(Buffer&& other);
	~Buffer();

	Buffer& operator=(Buffer&& other);

	void			reset();
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
