#pragma once

#include "Codec.h"
#include "PixelFormat.h"
#include "Rational.h"
#include "Resolution.h"

namespace Zuazo::Utils{

struct VideoMode{
	PixelFormat		pixFmt			=PixelFormats::PIX_FMT_NONE;
	Resolution		res				=Resolution(0, 0);
	Codec			codec			=Codecs::CODEC_NONE;
	Rational		frameRate		=Rational(0, 0);
	bool			progressive		=true;

	VideoMode()=default;
	VideoMode(const VideoMode& other)=default;
	~VideoMode()=default;

	constexpr int 	operator==(const VideoMode& other) const;
	constexpr int 	operator!=(const VideoMode& other) const;
	constexpr int 	operator<(const VideoMode& other) const;
	constexpr int 	operator>(const VideoMode& other) const;
	constexpr int 	operator<=(const VideoMode& other) const;
	constexpr int 	operator>=(const VideoMode& other) const;
};


constexpr int VideoMode::operator==(const VideoMode& other) const{
	return 	res==other.res &&
			pixFmt==other.pixFmt &&
			codec==other.codec &&
			progressive==other.progressive &&
			frameRate==other.frameRate;
}

constexpr int VideoMode::operator!=(const VideoMode& other) const{
	return 	!operator==(other);
}

constexpr int VideoMode::operator<(const VideoMode& other) const{
	if(res < other.res)
		return true;
	else if(res == other.res){
		if(pixFmt < other.pixFmt)
			return true;
		else if(pixFmt == other.pixFmt){
			if(codec < other.codec)
				return true;
			else if(codec == other.codec){
				if(progressive<other.progressive)
					return true;
				else{
					if(frameRate < other.frameRate)
						return true;
					else
						return false;
				}
			}else
				return false;
		}else
			return false;
	}else
		return false;
}

constexpr int VideoMode::operator>(const VideoMode& other) const{
	return !operator<(other) && !operator==(other);
}

constexpr int VideoMode::operator<=(const VideoMode& other) const{
	return operator<(other) || operator==(other);
}

constexpr int VideoMode::operator>=(const VideoMode& other) const{
	return !operator<(other);
}


}
