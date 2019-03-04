#include "Demuxer.h"

#include <memory>

using namespace Zuazo::Processors;

void Demuxer::open(){

	Consumer<Packet>::close();

	video.close();
	/* IMPLEMENT HERE OTHER MUXES*/

	Updateables::RegularUpdate<Updateables::UPDATE_ORDER_DEMUXER>::open();
}

void Demuxer::close(){
	Updateables::RegularUpdate<Updateables::UPDATE_ORDER_DEMUXER>::close();

	video.close();
	/* IMPLEMENT HERE OTHER MUXES*/

	Consumer<Packet>::close();
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
