#pragma once

#include "Signal/Layout.h"

namespace Zuazo {

class ZuazoBase : public Signal::Layout {
public:
	using Signal::Layout::Layout;

	virtual void open() = 0;
	virtual void close() = 0;
	bool isOpen() const;
private:
	bool    m_isOpen = false;
};

}

#include "ZuazoBase.inl"