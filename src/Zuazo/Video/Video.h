#pragma once

#include <mutex>

#include "../Utils/PixelTypes.h"
#include "../Stream/Source.h"
#include "../Stream/AsyncSource.h"
#include "../Stream/LazySource.h"
#include "../Stream/Consumer.h"
#include "../Stream/Delay.h"
#include "../Graphics/Frame.h"
#include "../UpdateOrder.h"

namespace Zuazo::Video{

/**
 * @brief //TODO
 */
typedef Stream::Source<Graphics::Frame> VideoSource;

/**
 * @brief //TODO
 */
typedef Stream::AsyncSource<Graphics::Frame> AsyncVideoSource;

/**
 * @brief //TODO
 */
typedef Stream::LazySource<Graphics::Frame> LazyVideoSource;

/**
 * @brief //TODO
 */
typedef Stream::Consumer<Graphics::Frame> VideoConsumer;

/**
 * @brief //TODO
 */
typedef Stream::Delay<Graphics::Frame> VideoDelay;

/**
 * @brief //TODO
 */
typedef AsyncVideoSource VideoInput;

/**
 * @brief //TODO
 */
class VideoOutput :
	public virtual VideoConsumer,
	public virtual Timing::PeriodicUpdate<UpdateOrder::OUTPUT>
{
protected:
	using Timing::PeriodicUpdate<UpdateOrder::OUTPUT>::setInterval;
	using Timing::PeriodicUpdate<UpdateOrder::OUTPUT>::setRate;

	virtual void open() override{
		VideoConsumer::open();
		Timing::PeriodicUpdate<UpdateOrder::OUTPUT>::open();
	}

	virtual void close() override{
		Timing::PeriodicUpdate<UpdateOrder::OUTPUT>::close();
		VideoConsumer::close();
	}
};
}
