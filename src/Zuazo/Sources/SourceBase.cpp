#include "SourceBase.h"

#include <map>

#include "../Consumers/ConsumerBase.h"

using namespace Zuazo::Sources;


SourceBase::~SourceBase(){
	while(m_consumers.size())
		(*m_consumers.begin())->setSource(nullptr);
}


void SourceBase::open(){
	Updateable::open();
}

void SourceBase::close(){
	m_last=std::shared_ptr<const Packet>();
	Updateable::close();
}

