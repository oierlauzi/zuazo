#include "ConsumerBase.h"

using namespace Zuazo::Stream;

ConsumerBase::ConsumerBase(){
	m_source=nullptr;
	m_lastPktId= -1;
}

ConsumerBase::ConsumerBase(const ConsumerBase& other) : ConsumerBase(){
	setSource(other.m_source);
}

ConsumerBase::~ConsumerBase(){
	setSource(nullptr);
}

void ConsumerBase::setSource(const SourceBase* src){
	std::lock_guard<std::mutex> lock(m_updateMutex);

	//Detach consumer from the previous source
	if(m_source)
		m_source->m_consumers.erase(this);

	//Attach the new consumer
	m_source=src;
	if(m_source)
		m_source->m_consumers.insert(this);
}
ConsumerBase& ConsumerBase::operator<<(const SourceBase& src){
	setSource(&src);
	return *this;
}

ConsumerBase& ConsumerBase::operator<<(std::nullptr_t ptr){
	setSource(nullptr);
	return *this;
}

void ConsumerBase::open(){
	Updateable::open();
}

void ConsumerBase::close(){
	Updateable::close();
}
