namespace Zuazo::Utils{

constexpr MMBuffer::MMBuffer() :
    m_data(nullptr),
    m_size(0)
{
}

inline MMBuffer::MMBuffer(int fd, size_t size, size_t offset, Prot prot, Flags flags) :
    m_data(mmap(nullptr, size, prot.to_ulong(), flags.to_ulong(), fd, offset)),
    m_size(size)
{
    if(!m_data) m_size = 0;
}

inline MMBuffer::MMBuffer(MMBuffer&& other) :
    m_data(other.m_data),
    m_size(other.m_size)
{
    other = MMBuffer();
}

inline MMBuffer::~MMBuffer(){
    if(m_data) munmap(m_data, m_size);
}

inline MMBuffer& MMBuffer::operator=(MMBuffer&& other){
    if(m_data) munmap(m_data, m_size);

    m_data = other.m_data;
	m_size = other.m_size;
    other.m_data = nullptr;
	other.m_size = 0;
}

inline void MMBuffer::reset(){
    (*this) = MMBuffer();
}

inline void* MMBuffer::get(){
    return m_data;
}

inline const void* MMBuffer::get() const{
    return m_data;
}

inline size_t MMBuffer::getSize() const{
    return m_size;
}

}