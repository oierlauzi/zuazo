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
	using OpenCallback = std::function<void(ZuazoBase&)>;
	using CloseCallback = std::function<void(ZuazoBase&)>;
	using UpdateCallback = std::function<void()>;

	ZuazoBase(	Instance& instance, 
				std::string name,
				OpenCallback openCbk = {},
				CloseCallback closeCbk = {},
				UpdateCallback updateCbk = {},
				std::initializer_list<PadRef> pads = {} );
	ZuazoBase(const ZuazoBase& other) = delete;
	ZuazoBase(ZuazoBase&& other) = default;
	virtual ~ZuazoBase() = default;

	ZuazoBase& 						operator=(const ZuazoBase& other) = delete;
	ZuazoBase& 						operator=(ZuazoBase&& other) = default;

	void 							open();
	void 							close();
	bool 							isOpen() const;

	Instance&						getInstance() const;

	void							setPreUpdateCallback(UpdateCallback cbk);
	const UpdateCallback&			setPreUpdateCallback() const;

	void							setPostUpdateCallback(UpdateCallback cbk);
	const UpdateCallback&			setPostUpdateCallback() const;

protected:
	void							setUpdateCallback(UpdateCallback cbk);
	const UpdateCallback&			setUpdateCallback() const;

	void							setOpenCallback(OpenCallback cbk);
	const OpenCallback&				setOpenCallback() const;

	void							setCloseCallback(CloseCallback cbk);
	const CloseCallback&			setCloseCallback() const;

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