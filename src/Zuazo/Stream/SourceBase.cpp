#include "SourceBase.h"

#include <map>

#include "ConsumerBase.h"

using namespace Zuazo::Stream;


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

