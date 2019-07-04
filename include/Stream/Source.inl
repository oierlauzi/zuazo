namespace Zuazo::Stream {

template <typename T>
inline typename Source<T>::id_t Source<T>::s_id = 0;

template <typename T>
inline std::shared_ptr<const T> Source<T>::get() const{
	return m_last;
}

template <typename T>
inline size_t Source<T>::getId() const{
	return m_lastId;
}

template <typename T>
inline void	Source<T>::push() const{
	m_last = std::shared_ptr<const T>();
    m_lastId = ++s_id;
}

template <typename T>
inline void	Source<T>::push(const std::shared_ptr<const T>& element) const {
	m_last = element;
    m_lastId = ++s_id;
}

template <typename T>
inline void	push(std::shared_ptr<const T>&& element) const {
	m_last = std::move(element);
    m_lastId = ++s_id;
}

template <typename T>
inline void	Source<T>::reset(){
	push();
}

}