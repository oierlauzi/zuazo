#pragma once

#include "Instance.h"
#include "Signal/Layout.h"

#include <utility>
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
	ZuazoBase(ZuazoBase&& other);
	virtual ~ZuazoBase();

	ZuazoBase& 						operator=(const ZuazoBase& other) = delete;
	ZuazoBase& 						operator=(ZuazoBase&& other);

	void 							open();
	void 							close();
	bool 							isOpen() const;

	Instance&						getInstance() const;

	void							setPreUpdateCallback(UpdateCallback cbk);
	const UpdateCallback&			getPreUpdateCallback() const;

	void							setPostUpdateCallback(UpdateCallback cbk);
	const UpdateCallback&			getPostUpdateCallback() const;

protected:
	void							setUpdateCallback(UpdateCallback cbk);
	const UpdateCallback&			getUpdateCallback() const;

	void							setMoveCallback(MoveCallback cbk);
	const MoveCallback&				getMoveCallback() const;

	void							setOpenCallback(OpenCallback cbk);
	const OpenCallback&				getOpenCallback() const;

	void							setCloseCallback(CloseCallback cbk);
	const CloseCallback&			getCloseCallback() const;

	void        					update() const;
	void							enableRegularUpdate(Instance::Priority prior) const;
	void							disableRegularUpdate() const;

	void							enablePeriodicUpdate(	Instance::Priority prior, 
															Duration period ) const;
	void							disablePeriodicUpdate() const;

private:
	enum Moments {
		PRE_UPDATE,
		UPDATE,
		POST_UPDATE,

		COUNT
	};

	using UpdateCallbacks = std::array<std::shared_ptr<UpdateCallback>, COUNT>;

	std::reference_wrapper<Instance>m_instance;

	bool    						m_isOpen;

	MoveCallback					m_moveCallback;
	OpenCallback					m_openCallback;
	CloseCallback					m_closeCallback;
	UpdateCallbacks					m_updateCallbacks;
	
};

#define ZUAZO_BASE_LOG(base, severity, message)														\
	if(	(base).getInstance().getApplicationInfo().elementLogFunc && 								\
		(((base).getInstance().getApplicationInfo().verbosity) & (severity)) != Verbosity::SILENT ) \
	{																								\
		(base).getInstance().getApplicationInfo().elementLogFunc(base, severity, message);			\
	}

}