#include "FFmpeg.h"

#include <sys/types.h>
#include <algorithm>
#include <chrono>
#include <ratio>

extern "C"{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/avutil.h>
	#include <libavutil/error.h>
	#include <libavutil/frame.h>
	#include <libavutil/imgutils.h>
	#include <libavutil/mathematics.h>
	#include <libavutil/version.h>
	#include <libswscale/swscale.h>
}

#include "../../Graphics/Context.h"
#include "../../Graphics/Frame.h"
#include "../../Utils/ImageAttributes.h"
#include "../../Utils/ImageBuffer.h"
#include "../../Utils/Resolution.h"

using namespace Zuazo::Media::Sources;


FFmpeg::FFmpeg(const std::string& dir)
	: m_file(dir)
{
	open();
}

FFmpeg::FFmpeg(const FFmpeg& other) : FFmpeg(other.m_file)
{
}

FFmpeg::~FFmpeg(){
	close();
}

void FFmpeg::open(){
	m_formatCtx = nullptr;
	m_codecCtx = nullptr;
	m_codec = nullptr;
	m_videoStream = -1;

	m_currTs=-1;

#if LIBAVUTIL_VERSION_MAJOR < 4
	//Register all supported codecs
	av_register_all();
#endif

	if(avformat_open_input(&m_formatCtx, m_file.c_str(), NULL, NULL)!=0)
		return;//Error opening the video file

	if(avformat_find_stream_info(m_formatCtx, NULL)<0)
		return; //Error getting stream info

	//Find a video stream
	for(u_int32_t i=0; i<m_formatCtx->nb_streams; i++){
		if(m_formatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO) {
			m_videoStream=i;
			break;
		}
	}
	if(m_videoStream == -1)
		return;	//Did not find a video stream

	//Find a codec for the video stream
	m_codec=avcodec_find_decoder(m_formatCtx->streams[m_videoStream]->codecpar->codec_id);
	if(!m_codec)
		return; //Unsupported codec

	//Create a codec context
	m_codecCtx = avcodec_alloc_context3(m_codec);
	avcodec_parameters_to_context(m_codecCtx, m_formatCtx->streams[m_videoStream]->codecpar);


	//Set up the multithreading
	m_codecCtx->thread_count=0; //0: Use all CPU cores

	if(m_codec->capabilities & AV_CODEC_CAP_FRAME_THREADS) //Check if codec supports FF_THREAD_FRAME
		m_codecCtx->thread_type |= FF_THREAD_FRAME;
	else if(m_codec->capabilities & AV_CODEC_CAP_FRAME_THREADS) //If not check if supports slice threads
		m_codecCtx->thread_type |= FF_THREAD_SLICE;

	// Open the codec
	if(avcodec_open2(m_codecCtx, m_codec, NULL)<0)
		return; //Error opening the codec



	int64_t duration=av_rescale_q(
			m_formatCtx->streams[m_videoStream]->duration,
			m_formatCtx->streams[m_videoStream]->time_base, AV_TIME_BASE_Q
	);

	setInfo(
			Timing::TimeInterval(std::chrono::duration<int64_t, std::ratio<1, AV_TIME_BASE>>(duration)),
			m_formatCtx->streams[m_videoStream]->nb_frames,
			Utils::Resolution(m_codecCtx->width, m_codecCtx->height)
	);

	VideoClip::open();

	m_exit=false;
	m_decodingThread=std::thread(&FFmpeg::decodingFunc, this);
}

void FFmpeg::close(){
	m_exit=true;
	m_decodeCondition.notify_one();
	if(m_decodingThread.joinable())
		m_decodingThread.join();

	VideoClip::close();

	if(m_formatCtx){
		avformat_close_input(&m_formatCtx);
		m_formatCtx=nullptr;
	}
	if(m_codecCtx){
		avcodec_close(m_codecCtx);
		m_codecCtx=nullptr;
	}
}

std::shared_ptr<const Zuazo::Graphics::Frame> FFmpeg::get() const{
	std::lock_guard<std::mutex> lock(m_decodeMutex);
	return VideoSource::get();
}

void FFmpeg::update() const{
	std::lock_guard<std::mutex> lock(m_decodeMutex);

	int64_t newTs=av_rescale_q(
			Clip::getCurrentTime().count(),
			ZUAZO_TIME_BASE_Q,
			m_formatCtx->streams[m_videoStream]->time_base
	);

	if(m_currTs != newTs){
		m_currTs=newTs;
		m_decodeCondition.notify_one();
	}
}

void FFmpeg::decodingFunc(){
	std::unique_lock<std::mutex> lock(m_decodeMutex);

	//Create the destination for the original frame
	AVFrame* decodedFrame=av_frame_alloc();

	if(!decodedFrame)
		return;

	//Create the destination for the rgba (final) frame
	AVFrame* finalFrame=av_frame_alloc();

	if(!finalFrame){
		av_frame_unref(decodedFrame);
		return;
	}


	//Set up everything for conversion between color spaces
	AVPixelFormat pixFmt=findBestMatch(m_codecCtx->pix_fmt);
	SwsContext* swsCtx= sws_getContext(
			m_codecCtx->width,
			m_codecCtx->height,
			m_codecCtx->pix_fmt,
			m_codecCtx->width,
			m_codecCtx->height,
			pixFmt,
			SWS_POINT,
			NULL,
			NULL,
			NULL);

	if(!swsCtx){
		av_frame_unref(decodedFrame);
		av_frame_unref(finalFrame);
		return;
	}


	Utils::ImageAttributes imgAtt(
			Utils::Resolution(m_codecCtx->width, m_codecCtx->height),
			toPixelTypes(pixFmt)
	);

	Utils::ImageBuffer imgBuf(imgAtt);

	//Set up the destination frame
	av_image_fill_arrays(
			finalFrame->data,
			finalFrame->linesize,
			imgBuf.data,
			pixFmt,
			m_codecCtx->width,
			m_codecCtx->height,
			1
	);

	int64_t lastTs=m_formatCtx->streams[m_videoStream]->duration;
	int64_t decodedTs=-1;
	int64_t decodedDur=-1;

	while(!m_exit){
		//check if the timestamp is larger than the duration
		if(m_currTs > lastTs)
			m_currTs=lastTs;


		//Evaluate if seeking is needed
		int64_t deltaTs=m_currTs - decodedTs;
		if(deltaTs < 0 || deltaTs >= 20 || decodedTs < 0){
			if(av_seek_frame(m_formatCtx, m_videoStream, m_currTs, AVSEEK_FLAG_BACKWARD) < 0)
				return;

			decodedTs=-1;

			//Enter draining mode
			avcodec_send_packet(m_codecCtx, NULL);

			//Empty the buffer
			while(avcodec_receive_frame(m_codecCtx, decodedFrame) != AVERROR_EOF)
				continue;

			//Flush the buffers in the codec
			avcodec_flush_buffers(m_codecCtx);
		}

		while((decodedTs + decodedDur) < m_currTs){
			//Try to decode a frame
			int err=avcodec_receive_frame(m_codecCtx, decodedFrame);

			if(err==0){
				//Frame has been successfully decoded.
				//Save it's timing values for future use cases
				decodedTs=decodedFrame->pts;
				decodedDur=decodedFrame->pkt_duration;
			}else if(err==AVERROR(EAGAIN)){
				//We need at least 1 packet to be able to decode the frame
				AVPacket packet;

				//Read packet from file
				if(av_read_frame(m_formatCtx, &packet)<0){
					//File has ended
					//Enter draining mode
					avcodec_send_packet(m_codecCtx, NULL);
					//To escape from draining mode, Clip needs to be seeked
				}else{
					//It needs to be a packet from the video stream
					if(packet.stream_index==m_videoStream){
						//Send the packet
						if(avcodec_send_packet(m_codecCtx, &packet)<0){
							//Something went wrong sending the packet
							av_packet_unref(&packet);
							break;
						}
					}

					av_packet_unref(&packet);
					}
			}else
				break; //Error decoding the frame
		}

		//Convert the frame to the final one
		sws_scale(
				swsCtx,
				(uint8_t const * const *)decodedFrame->data,
				decodedFrame->linesize,
				0,
				decodedFrame->height,
				finalFrame->data,
				finalFrame->linesize
		);

		//Upload image to the source
		std::unique_ptr<const Graphics::Frame> newFrame;

		{
			Graphics::UniqueContext ctx(Graphics::Context::getAvalibleCtx());

			newFrame=std::unique_ptr<Graphics::Frame>(
					new Graphics::Frame(imgBuf)
			);
		}

		VideoSource::push(std::move(newFrame));

		m_decodeCondition.wait(lock);
	}

	//Free everything
	av_frame_unref(decodedFrame);
	av_frame_unref(finalFrame);
	sws_freeContext(swsCtx);
}

AVPixelFormat FFmpeg::findBestMatch(AVPixelFormat fmt){
	AVPixelFormat result;

	switch(fmt){
	case AV_PIX_FMT_RGB24:
		result=AV_PIX_FMT_RGB24;
		break;
	case AV_PIX_FMT_BGR24:
		result=AV_PIX_FMT_BGR24;
		break;
	case AV_PIX_FMT_ARGB:
		result=AV_PIX_FMT_RGBA;
		break;
	case AV_PIX_FMT_RGBA:
		result=AV_PIX_FMT_RGBA;
		break;
	case AV_PIX_FMT_ABGR:
		result=AV_PIX_FMT_BGRA;
		break;
	case AV_PIX_FMT_BGRA:
		result=AV_PIX_FMT_BGRA;
		break;
	default:
		const AVPixFmtDescriptor* fmtDesc=av_pix_fmt_desc_get(fmt);
		result=fmtDesc->flags & AV_PIX_FMT_FLAG_ALPHA
				? AV_PIX_FMT_RGBA
				: AV_PIX_FMT_RGB24;
	}

	return result;
}

Zuazo::Utils::PixelTypes FFmpeg::toPixelTypes(AVPixelFormat fmt){
	Utils::PixelTypes  result;

	switch(fmt){
	case AV_PIX_FMT_BGR24:
		result=Utils::PixelTypes::BGR;
		break;
	case AV_PIX_FMT_RGB24:
		result=Utils::PixelTypes::RGB;
		break;
	case AV_PIX_FMT_RGBA:
		result=Utils::PixelTypes::RGBA;
		break;
	case AV_PIX_FMT_BGRA:
		result=Utils::PixelTypes::BGRA;
		break;
	default:
		result=Utils::PixelTypes::NONE;
	}

	return result;
}
