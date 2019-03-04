#pragma once

#include "../Graphics/Frame.h"
#include "../Packet.h"
#include "../Stream/Consumer.h"
#include "../Stream/Source.h"
#include "../Updateables/RegularUpdate.h"
#include "../Updateables/UpdateOrder.h"

namespace Zuazo::Processors{
class Demuxer :
		public Stream::Consumer<Packet>,
		public Updateables::RegularUpdate<Updateables::UPDATE_ORDER_DEMUXER>
{
public:
	Demuxer(){open();}
	Demuxer(const Demuxer&)=delete;
	Demuxer(Demuxer&&)=default;
	~Demuxer(){close();}

	template <typename T>
	class DemuxerOutput :
			public Stream::Source<T>
	{
		friend Demuxer;
	public:
		using Stream::Source<T>::Source;
	private:
		void update() const override{}

		using Stream::Source<T>::open;
		using Stream::Source<T>::close;
		using Stream::Source<T>::push;
	};

	DemuxerOutput<Graphics::Frame>			video;
	/* IMPLEMENT HERE OTHER MUXES*/

	void									open() override;
	void									close() override;
protected:
	void									update() const override;

};
}
