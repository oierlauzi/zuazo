#pragma once

#include "../Graphics/Frame.h"
#include "../Packet.h"
#include "../Stream/Consumer.h"
#include "../Stream/LazySource.h"

namespace Zuazo::Processors{
class Demuxer :
		public Stream::Consumer<Packet>
{
public:
	Demuxer();
	Demuxer(const Demuxer&)=delete;
	Demuxer(Demuxer&&)=default;
	~Demuxer(){close();}

	template <typename T>
	class DemuxerOutput :
			public Stream::LazySource<T>
	{
		friend Demuxer;
	public:
		using Stream::LazySource<T>::Source;
	private:
		const Demuxer& owner;

		DemuxerOutput(const Demuxer& owner) : owner(owner){}
		DemuxerOutput(const DemuxerOutput& owner)=default;
		~DemuxerOutput()=default;

		void update() const override{owner.update();}

		using Stream::LazySource<T>::open;
		using Stream::LazySource<T>::close;
		using Stream::LazySource<T>::push;
	};

	DemuxerOutput<Graphics::Frame>			video;
	/* IMPLEMENT HERE OTHER MUXES*/

	void									open() override;
	void									close() override;
protected:
	void									update() const override;

};
}
