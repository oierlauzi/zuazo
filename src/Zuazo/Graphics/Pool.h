#pragma once

#include <map>
#include <queue>
#include <memory>
#include <mutex>

#include "GL/Context.h"
#include "GL/Texture2D.h"
#include "GL/FrameBuffer.h"
#include "GL/PixelIO.h"
#include "../Utils/Resolution.h"

namespace Zuazo::Graphics{

/**
 * @brief A pool for GL related classes ordered by a key (usually resolution)
 * to avoid expensive construction/destruction and reallocations
 */
template<typename T, typename Q>
class Pool{
public:
	constexpr Pool(u_int32_t elements=32);
	Pool(const Pool& other)=delete;
	~Pool()=default;

	std::unique_ptr<Q>								pop(const T& ref);
	void											push(const T& ref, std::unique_ptr<Q>& el);

	static Pool<T, Q>								s_globalPool;
private:
	u_int32_t										m_nMaxElements;
	std::map<T, std::queue<std::unique_ptr<Q>>>		m_elements;
	std::mutex										m_mutex;
};

/**
 * @brief A pool holding available 2D textures
 */
template<Utils::PixelTypes type>
using Texture2DPool=Pool<Utils::Resolution, GL::Texture2D<type>>;

/**
 * @brief A pool holding available pixel unpack buffers
 */
template<Utils::PixelTypes type>
using PixelUnpackPool=Pool<Utils::Resolution, GL::PixelUnpackBuffer<type>>;

/**
 * @brief A pool holding available framebuffers
 */
using FrameBufferPool=Pool<Utils::Resolution, GL::FrameBuffer>;

/*
 * STATIC VARIABLE DECLARATIONS
 */
template<typename T, typename Q>
inline Pool<T, Q> Pool<T, Q>::s_globalPool;

/*
 * METHOD DEFINITIONS
 */

template<typename T, typename Q>
constexpr Pool<T, Q>::Pool(u_int32_t elements) : m_nMaxElements(elements){

}

template<typename T, typename Q>
inline std::unique_ptr<Q> Pool<T, Q>::pop(const T& ref){
	std::unique_ptr<Q> result;
	std::lock_guard<std::mutex> lock(m_mutex);

	if(m_elements[ref].size()){
		//Requested element.exists
		result=std::move(m_elements[ref].front());
		m_elements[ref].pop();
	}else{
		//Requested element does not exist -> construct it
		GL::UniqueContext ctx(GL::Context::mainCtx);
		result=std::unique_ptr<Q>(new Q());
	}

	return result;
}

template<typename T, typename Q>
inline void Pool<T, Q>::push(const T& ref, std::unique_ptr<Q>& el){
	std::lock_guard<std::mutex> lock(m_mutex);

	if(m_elements[ref].size() < m_nMaxElements){
		//There is room for another element
		m_elements[ref].push(std::move(el));
	}else{
		//There is no room for another element
		el.reset();
	}
}

}
