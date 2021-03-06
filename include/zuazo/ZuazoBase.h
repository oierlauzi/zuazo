#pragma once

#include "Instance.h"
#include "Utils/Pimpl.h"
#include "Utils/BufferView.h"
#include "Signal/Layout.h"

#include <functional>
#include <mutex>

namespace Zuazo {

class ZuazoBase 
	: public Signal::Layout
{
public:
	using MoveCallback = std::function<void(ZuazoBase&)>;
	using OpenCallback = std::function<void(ZuazoBase&)>;
	using AsyncOpenCallback = std::function<void(ZuazoBase&, std::unique_lock<Instance>&)>;
	using CloseCallback = std::function<void(ZuazoBase&)>;
	using AsyncCloseCallback = std::function<void(ZuazoBase&, std::unique_lock<Instance>&)>;
	using UpdateCallback = std::function<void()>;

	ZuazoBase(	Instance& instance, 
				std::string name,
				Utils::BufferView<const PadRef> pads = {},
				MoveCallback moveCbk = {},
				OpenCallback openCbk = {},
				AsyncOpenCallback asyncOpenCbk = {},
				CloseCallback closeCbk = {},
				AsyncCloseCallback asyncCloseCbk = {},
				UpdateCallback updateCbk = {} );
	ZuazoBase(const ZuazoBase& other) = delete;
	ZuazoBase(ZuazoBase&& other) noexcept;
	virtual ~ZuazoBase();

	ZuazoBase& 						operator=(const ZuazoBase& other) = delete;
	ZuazoBase& 						operator=(ZuazoBase&& other) noexcept;

	void 							open();
	void							asyncOpen(std::unique_lock<Instance>& lock);
	void 							close();
	void							asyncClose(std::unique_lock<Instance>& lock);
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
		(((base).getInstance().getApplicationInfo().getVerbosity()) & (severity)) != Verbosity::silent )\
	{																									\
		(base).getInstance().getApplicationInfo().getElementLogFunc()(base, severity, message);			\
	}

}