#pragma once

#include <sys/types.h>
#include <cstddef>
#include <memory>

#include "../Updateable.h"
#include "Packet.h"
#include "SourceBase.h"

namespace Zuazo::Stream{

class ConsumerBase :
	public virtual Updateable
{
	friend SourceBase;
public:
	ConsumerBase();
	ConsumerBase(const ConsumerBase& other);
	virtual ~ConsumerBase();

	void							setSource(const SourceBase* src);
	ConsumerBase&					operator<<(const SourceBase& src);
	ConsumerBase&					operator<<(std::nullptr_t ptr);

	virtual void					open() override;
	virtual void					close() override;
protected:
	std::shared_ptr<const Packet>	get() const;
	bool							hasChanged() const;

private:
	const SourceBase*				m_source;

	mutable int64_t					m_lastPktId;

	std::shared_ptr<const Packet>	reqPacket() const;
};


inline std::shared_ptr<const Packet> ConsumerBase::get() const{
	std::shared_ptr<const Packet> pkt=reqPacket();

	if(pkt)
		m_lastPktId=pkt->id;
	else
		m_lastPktId=-1;

	return pkt;
}

inline bool ConsumerBase::hasChanged() const{
	std::shared_ptr<const Packet> pkt=reqPacket();

	if(pkt)
		return reqPacket()->id != m_lastPktId;
	else
		return m_lastPktId != -1;

}

inline std::shared_ptr<const Packet> ConsumerBase::reqPacket() const{
	if(m_source)
		return m_source->get();
	else
		return std::shared_ptr<const Packet>(); //No source, return an empty ptr
}

}
