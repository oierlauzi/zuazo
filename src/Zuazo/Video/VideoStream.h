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

	template<typename Q>
	using VideoSourcePad=Stream::SourcePad<VideoElement, Q>;
	template<typename Q>
	using VideoConsumerPad=Stream::ConsumerPad<VideoElement, Q>;
	template<typename Q>
	using AsyncVideoSourcePad=Stream::AsyncSourcePad<VideoElement, Q>;
	template<typename Q>
	using LazyVideoSourcePad=Stream::LazySourcePad<VideoElement, Q>;

	template<typename Q>
	using DummyBidirVideoPad=Stream::DummyBidirPad<VideoElement, Q>;
	template<typename Q>
	using DummyInputVideoPad=Stream::DummyInputPad<VideoElement, Q>;
	template<typename Q>
	using DummyOutputVideoPad=Stream::DummyOutputPad<VideoElement, Q>;
}
