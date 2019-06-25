#pragma once

#define PROT_BITS 3
#define FLAG_BITS 32

extern "C"{
    #include <stddef.h>
    #include <stdlib.h>
    #include <sys/mman.h>
}

#include <bitset>

namespace Zuazo::Utils {

class MMBuffer{
public:
    typedef std::bitset<PROT_BITS> Prot;
    typedef std::bitset<FLAG_BITS> Flags;

    constexpr MMBuffer();
    MMBuffer(int fd, size_t size, size_t offset = 0, Prot prot = PROT_NONE, Flags flags = 0);
    MMBuffer(const MMBuffer& other) = delete;
    MMBuffer(MMBuffer&& other);
    ~MMBuffer();

    MMBuffer& operator=(MMBuffer&& other);

    void            reset();

    void*			get();
	const void*		get() const;
	size_t			getSize() const;
private:
    void*   m_data;
    size_t  m_size;
};

}

#include "MMBuffer.inl"
