#include "LazySourceBase.h"

using namespace Zuazo::Sources;

std::shared_ptr<const Zuazo::Packet> LazySourceBase::get() const{
	//Only update if it is not being updated (to avoid mutex deadlocks and endless loops)
	if(!m_updateInProgress){
		m_updateInProgress=true;
		Updateable::perform();
		m_updateInProgress=false;
	}
	return SourceBase::get();
}


void LazySourceBase::open(){
	SourceBase::open();
}


void LazySourceBase::close(){
	SourceBase::close();
}
