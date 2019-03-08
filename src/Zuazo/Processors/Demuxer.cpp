#include "Demuxer.h"

#include <memory>

using namespace Zuazo::Processors;

Demuxer::Demuxer():
	video(*this)
	/* IMPLEMENT HERE OTHER MUXES*/
{
	open();
}

void Demuxer::open(){
	Consumer<Packet>::close();

	video.close();
	/* IMPLEMENT HERE OTHER MUXES*/

	Updateables::Updateable::open();
}

void Demuxer::close(){
	video.close();
	/* IMPLEMENT HERE OTHER MUXES*/

	Consumer<Packet>::close();
	Updateables::Updateable::close();
}

void Demuxer::update() const{
	if(Consumer<Packet>::hasChanged()){
		std::shared_ptr<const Packet> pkt=Consumer<Packet>::get();
		if(pkt){
			video.push(pkt->frame);
			/* IMPLEMENT HERE OTHER MUXES*/
		}else{
			video.push();
			/* IMPLEMENT HERE OTHER MUXES*/
		}
	}
}
