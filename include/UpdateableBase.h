#pragma once

#include <sys/types.h>

namespace Zuazo{

class UpdateableBase{
public:
	typedef int32_t UpdatePriority;

	UpdateableBase();
	UpdateableBase(UpdatePriority prior);
	UpdateableBase(const UpdateableBase& other)=default;
	UpdateableBase(UpdateableBase&& other)=default;
	virtual ~UpdateableBase()=default;

	UpdatePriority			getPriority() const;

	virtual void			update() const=0;

	virtual void 			enable()=0;
	virtual void 			disable()=0;
protected:
	const UpdatePriority	m_updatePriority;
};

namespace UpdatePriorities{
	using UpdatePriority=UpdateableBase::UpdatePriority;

	constexpr UpdatePriority 	DONT_CARE	=-1;

	constexpr UpdatePriority 	ANIMATION	=0;	

	constexpr UpdatePriority 	SOURCE		=1;

	constexpr UpdatePriority 	DELAY		=2;

	constexpr UpdatePriority 	CONSUMER	=3;

	constexpr UpdatePriority 	FF_DEC		=4;
}

inline UpdateableBase::UpdateableBase() :
	m_updatePriority(UpdatePriorities::DONT_CARE)
{
}

inline UpdateableBase::UpdateableBase(UpdatePriority prior) :
	m_updatePriority(prior)
{
}

inline UpdateableBase::UpdatePriority UpdateableBase::getPriority() const{
	return m_updatePriority;
}

}
