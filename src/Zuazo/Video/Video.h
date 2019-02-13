#pragma once

#include <mutex>

#include "../Utils/PixelTypes.h"
#include "../Stream/Source.h"
#include "../Stream/AsyncSource.h"
#include "../Stream/LazySource.h"
#include "../Stream/Consumer.h"
#include "../Stream/Delay.h"
#include "../Graphics/Frame.h"

namespace Zuazo::Video{

/**
 * @brief //TODO
 */
typedef Stream::Source<Graphics::Frame> VideoSource;

/**
 * @brief //TODO
 */
typedef Stream::AsyncSource<Graphics::Frame> VideoAsyncSource;

/**
 * @brief //TODO
 */
typedef Stream::LazySource<Graphics::Frame> VideoLazySource;

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
class IndependentVideoConsumer{
public:
	IndependentVideoConsumer()=default;
	IndependentVideoConsumer(const IndependentVideoConsumer& other)=default;
	~IndependentVideoConsumer()=default;

	VideoConsumer fill;
	VideoConsumer key;

	IndependentVideoConsumer& operator<<(const VideoSource& src){
		fill.setSource(&src);
		key.setSource(&src);
		return *this;
	}

	IndependentVideoConsumer& operator<<(std::nullptr_t ptr){
		fill.setSource(nullptr);
		key.setSource(nullptr);
		return *this;
	}
};

}
