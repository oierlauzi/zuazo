#pragma once

#include "../Graphics/Frame.h"
#include "../Stream/AsyncSource.h"
#include "../Stream/Consumer.h"
#include "../Stream/Delay.h"
#include "../Stream/DummyPad.h"
#include "../Stream/LazySource.h"
#include "../Stream/Source.h"

namespace Zuazo::Video{
	typedef Graphics::Frame							VideoElement;
	typedef Stream::Source<VideoElement>			VideoSource;
	typedef Stream::Consumer<VideoElement>			VideoConsumer;
	typedef Stream::Delay<VideoElement>				VideoDelay;

	typedef Stream::AsyncSource<VideoElement>		VideoAsyncSource;
	typedef Stream::LazySource<VideoElement>		VideoLazySource;

	typedef Stream::SourcePad<VideoElement> 		VideoSourcePad;
	typedef Stream::ConsumerPad<VideoElement>		VideoConsumerPad;
	typedef Stream::AsyncSourcePad<VideoElement>	AsyncVideoSourcePad;
	typedef Stream::LazySourcePad<VideoElement>		LazyVideoSourcePad;
	typedef Stream::DummyPad<VideoElement>			DummyVideoPad;
}
