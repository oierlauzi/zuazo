#pragma once

#include <sys/types.h>
#include <memory>
#include <queue>

#include "../Timing/PeriodicUpdate.h"
#include "../UpdateOrder.h"
#include "../Utils/Rational.h"
#include "SourceBase.h"

namespace Zuazo::Sources{

class AsyncSourceBase :
		public SourceBase,
		public Timing::PeriodicUpdate< (u_int32_t)UpdateOrder::INPUT >
{
public:
	AsyncSourceBase();
	AsyncSourceBase(const Utils::Rational& rat);
	AsyncSourceBase(const AsyncSourceBase& other)=delete;
	virtual ~AsyncSourceBase()=default;

	void								setMaxDropped(u_int32_t max);
	u_int32_t							getMaxDropped() const;
	u_int32_t							getDropped() const;
	void								resetDropped();

	void								setMaxBufferSize(u_int32_t size);
	u_int32_t							getMaxBufferSize() const;
	u_int32_t							getBufferSize() const;
	void								flushBuffer();

	virtual void						open() override;
	virtual void						close() override;
protected:
	void								push(std::unique_ptr<const Packet> element);

	virtual void 						update() const override;
private:
	static constexpr u_int32_t			DEFAULT_MAX_DROPPED=3;
	static constexpr u_int32_t			DEFAULT_MAX_BUFFER_SIZE=3;

	u_int32_t							m_maxDropped;
	mutable u_int32_t					m_dropped;

	u_int32_t							m_maxBufferSize;
	mutable std::queue<std::unique_ptr<const Packet>> m_buffer;
};



}
