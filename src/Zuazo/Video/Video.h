#pragma once

#include "../Graphics/Frame.h"
#include "../Stream/AsyncSource.h"
#include "../Stream/Consumer.h"
#include "../Stream/Delay.h"
#include "../Stream/LazySource.h"
#include "../Stream/Source.h"
#include "../Stream/SyncConsumer.h"

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
typedef Stream::SyncConsumer<Graphics::Frame> SyncVideoConsumer;

/**
 * @brief //TODO
 */
typedef Stream::Delay<Graphics::Frame> VideoDelay;

class Resizeable{
public:
	virtual ~Resizeable()=default;

	virtual void setRes(const Utils::Resolution& res)=0;
	virtual Utils::Resolution getRes() const=0;
};

class VideoScaler{
public:
	virtual ~VideoScaler()=default;

	virtual void setScalingMode(Utils::ScalingModes mode)=0;
	virtual Utils::ScalingModes getScalingMode() const=0;
};



}
