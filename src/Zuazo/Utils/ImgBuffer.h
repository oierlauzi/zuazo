#pragma once

#include <stddef.h>
#include <sys/types.h>
#include <memory>

#include "Resolution.h"

namespace Zuazo{

/*
 * It is used to exchange images with Zuazo
 */
struct ImgBuffer{
	Resolution res;
	std::unique_ptr<u_int8_t[]> data;

	ImgBuffer(){

	}

	ImgBuffer(const Resolution& res){
		this->res=res;
		data=std::unique_ptr<u_int8_t[]>( new u_int8_t[getSize()] );
	}

	ImgBuffer(const Resolution& res, std::unique_ptr<u_int8_t[]> data){
		this->res=res;
		this->data=std::move(data);
	}

	size_t getSize(){
		return res.width * res.height * 4;
	}
};

}
