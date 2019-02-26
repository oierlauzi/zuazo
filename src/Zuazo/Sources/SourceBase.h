#pragma once

#include <algorithm>
#include <memory>
#include <set>

#include "../Packet.h"
#include "../Updateable.h"

namespace Zuazo::Consumers{
	class ConsumerBase;
}

namespace Zuazo::Sources{

class SourceBase :
		public virtual Updateable
{
	friend Consumers::ConsumerBase;
public:
	SourceBase()=default;
	SourceBase(const SourceBase& other)=delete;
	SourceBase(SourceBase&& other);
	virtual ~SourceBase();

	virtual std::shared_ptr<const Packet>	get() const;
protected:
	void									push() const;
	void									push(std::shared_ptr<const Packet>& element) const;
	void									push(std::unique_ptr<const Packet> element) const;

	virtual void							open() override;
	virtual void							close() override;
private:
	mutable std::shared_ptr<const Packet>	m_last;

	mutable std::set<Consumers::ConsumerBase*> m_consumers;
};


/*
 * INLINE METHOD DEFINITIONS
 */

inline std::shared_ptr<const Packet> SourceBase::get() const{
	return m_last;
}

inline void	SourceBase::push() const{
	m_last=std::shared_ptr<const Packet>();
}

inline void	SourceBase::push(std::shared_ptr<const Packet>& element) const{
	m_last=element;
}

inline void	SourceBase::push(std::unique_ptr<const Packet> element) const{
	m_last=std::move(element);
}
}
