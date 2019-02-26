#pragma once

#include "SourceBase.h"

namespace Zuazo::Sources{

class LazySourceBase :
		public SourceBase
{
public:
	using SourceBase::SourceBase;

	virtual std::shared_ptr<const Packet>	get() const override;

	virtual void							open() override;
	virtual void							close() override;
private:
	mutable bool							m_updateInProgress=false;
};


}
