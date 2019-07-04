namespace Zuazo::Stream {

template <typename T>
inline Consumer<T>::Consumer() :
	m_source(nullptr),
    m_lastId(0)
{
}

template <typename T>
inline void Consumer<T>::setSource(const Source<T>* src){
	m_source=src;
}

template <typename T>
inline std::shared_ptr<const T> Consumer<T>::get() const{
    m_lastId = getId();
    return m_source ? m_source->get() : std::shared_ptr<const T>();
}

template <typename T>
inline typename Source<T>::id_t Consumer<T>::getId() const{
    return m_source ? m_source->getId() : 0;
}

template <typename T>
inline bool Consumer<T>::hasChanged() const{
	return m_lastId == getId(); 

}

template <typename T>
inline void Consumer<T>::reset(){
	m_lastElement=std::shared_ptr<const T>();
}

template <typename T>
inline std::shared_ptr<const T> Consumer<T>::reqElement() const{
	
}

}