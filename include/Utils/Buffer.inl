namespace Zuazo::Utils {

inline Buffer::Buffer() :
		m_data(nullptr),
		m_size(0)
{
}

inline Buffer::Buffer(size_t size) :
		m_data(malloc(size)),
		m_size(size)
{
}

inline Buffer::Buffer(size_t size, const void* data) :
		Buffer(size)
{
	memcpy(m_data, data, size);
}

inline Buffer::Buffer(const Buffer& other) :
		Buffer(other.m_size, other.m_data)
{
}

inline Buffer::Buffer(Buffer&& other) :
		m_data(other.m_data),
		m_size(other.m_size)
{
    other.m_data=nullptr;
    other.m_size=0;
}

inline Buffer::~Buffer(){
	if(m_data)
		free(m_data);
}

inline void Buffer::reallocate(size_t size){
	m_size=size;
	m_data = realloc(m_data, m_size);
}

inline void* Buffer::get(){
	return m_data;
}

inline const void* Buffer::get() const{
	return m_data;
}

inline size_t Buffer::getSize() const{
	return m_size;
}

}