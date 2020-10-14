#pragma once

#include "Instance.h"
#include "Utils/Pimpl.h"
#include "Signal/Layout.h"

#include <functional>

namespace Zuazo {

class ZuazoBase 
	: public Signal::Layout
{
public:
	using MoveCallback = std::function<void(ZuazoBase&)>;
	using OpenCallback = std::function<void(ZuazoBase&)>;
	using CloseCallback = std::function<void(ZuazoBase&)>;
	using UpdateCallback = std::function<void()>;

	ZuazoBase(	Instance& instance, 
				std::string name,
				std::initializer_list<PadRef> pads = {},
				MoveCallback moveCbk = {},
				OpenCallback openCbk = {},
				CloseCallback closeCbk = {},
				UpdateCallback updateCbk = {} );
	ZuazoBase(const ZuazoBase& other) = delete;
	ZuazoBase(ZuazoBase&& other) noexcept;
	virtual ~ZuazoBase();

	ZuazoBase& 						operator=(const ZuazoBase& other) = delete;
	ZuazoBase& 						operator=(ZuazoBase&& other) noexcept;

	void 							open();
	void 							close();
	bool 							isOpen() const noexcept;

	Instance&						getInstance() const noexcept;

	void							setPreUpdateCallback(UpdateCallback cbk) noexcept;
	const UpdateCallback&			getPreUpdateCallback() const noexcept;

	void							setPostUpdateCallback(UpdateCallback cbk) noexcept;
	const UpdateCallback&			getPostUpdateCallback() const noexcept;

protected:
	void							setUpdateCallback(UpdateCallback cbk) noexcept;
	const UpdateCallback&			getUpdateCallback() const noexcept;

	void							setMoveCallback(MoveCallback cbk) noexcept;
	const MoveCallback&				getMoveCallback() const noexcept;

	void							setOpenCallback(OpenCallback cbk) noexcept;
	const OpenCallback&				getOpenCallback() const noexcept;

	void							setCloseCallback(CloseCallback cbk) noexcept;
	const CloseCallback&			getCloseCallback() const noexcept;

	void        					update() const noexcept;
	void							enableRegularUpdate(Instance::Priority prior) const;
	void							disableRegularUpdate() const noexcept;

	void							enablePeriodicUpdate(	Instance::Priority prior, 
															Duration period ) const;
	void							disablePeriodicUpdate() const noexcept;

private:
	struct Impl;
	Utils::Pimpl<Impl>				m_impl;
	
};

#define ZUAZO_BASE_LOG(base, severity, message)															\
	if(	(base).getInstance().getApplicationInfo().getElementLogFunc() && 								\
		(((base).getInstance().getApplicationInfo().getVerbosity()) & (severity)) != Verbosity::SILENT )\
	{																									\
		(base).getInstance().getApplicationInfo().getElementLogFunc()(base, severity, message);			\
	}

}