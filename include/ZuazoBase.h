#pragma once

#include "Instance.h"
#include "Signal/Layout.h"

#include <utility>

namespace Zuazo {

class ZuazoBase 
	: public Signal::Layout {
public:
	template<typename Str>
	ZuazoBase(const Instance& instance, Str&& name);
	ZuazoBase(const ZuazoBase& other) = delete;
	ZuazoBase(ZuazoBase&& other) = default;
	virtual ~ZuazoBase() = default;

	ZuazoBase& 								operator=(const ZuazoBase& other) = delete;
	ZuazoBase& 								operator=(ZuazoBase&& other) = default;

	const Instance&							getInstance() const;

	virtual void 							open() = 0;
	virtual void 							close() = 0;
	bool 									isOpen() const;

private:
	std::reference_wrapper<const Instance> 	m_instance;

	bool    								m_isOpen;
	
};

}

#include "ZuazoBase.inl"