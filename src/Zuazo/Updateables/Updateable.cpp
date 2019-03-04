#include "Updateable.h"

using namespace Zuazo::Updateables;

Updateable::Updateable(){
	m_beforeCbk=nullptr;
	m_afterCbk=nullptr;
	m_isOpen=false;
}


Updateable::Updateable(const Updateable& other){
	m_beforeCbk=other.m_beforeCbk;
	m_afterCbk=other.m_afterCbk;
	m_isOpen=other.m_isOpen;
}

void Updateable::perform() const{
	std::lock_guard<std::mutex> lock(m_mutex);
	if(m_beforeCbk)
		m_beforeCbk->update(this);

	{
	std::lock_guard<std::mutex> lock(m_updateMutex);
	update();
	}

	if(m_afterCbk)
		m_afterCbk->update(this);
}
