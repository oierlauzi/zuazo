#pragma once

#include <sys/types.h>

namespace Zuazo::Timing{

constexpr u_int32_t UPDATE_ORDER_SOURCE		=1;

constexpr u_int32_t UPDATE_ORDER_DELAY		=2;
constexpr u_int32_t UPDATE_ORDER_UPLOADER	=2;
constexpr u_int32_t UPDATE_ORDER_DEMUXER	=2;
constexpr u_int32_t UPDATE_ORDER_MUXER		=2;

constexpr u_int32_t UPDATE_ORDER_CONSUMER	=3;

constexpr u_int32_t UPDATE_ORDER_POOL		=4;
constexpr u_int32_t UPDATE_ORDER_FF_DEC		=4;

}
