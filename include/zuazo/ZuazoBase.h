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
	using UpdateCallback = std::function<void()>;

	template<typename Str, typename Pads>
	ZuazoBase(Instance& instance, Str&& name, Pads&& pads);
	ZuazoBase(const ZuazoBase& other) = delete;
	ZuazoBase(ZuazoBase&& other) = default;
	virtual ~ZuazoBase();

	ZuazoBase& 						operator=(const ZuazoBase& other) = delete;
	ZuazoBase& 						operator=(ZuazoBase&& other) = default;

	virtual void 					open() = 0;
	virtual void 					close() = 0;
	bool 							isOpen() const;

	Instance&						getInstance() const;

	template <typename Cbk>
	void							setPreUpdateCallback(Cbk&& cbk);
	const UpdateCallback&			setPreUpdateCallback() const;

	template <typename Cbk>
	void							setPostUpdateCallback(Cbk&& cbk);
	const UpdateCallback&			setPostUpdateCallback() const;

protected:
	template <typename Cbk>
	void							setUpdateCallback(Cbk&& cbk);
	const UpdateCallback&			setUpdateCallback() const;

	template<typename Func>
	void							visitCallbacks(const Func& func) const;
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

	using Callbacks = std::array<std::shared_ptr<UpdateCallback>, COUNT>; 

	std::reference_wrapper<Instance>m_instance;

	bool    						m_isOpen;

	Callbacks						m_callbacks;
	
};

#define ZUAZO_BASE_LOG(base, severity, message)														\
	if(	(base).getInstance().getApplicationInfo().elementLogFunc && 								\
		((severity) & (base).getInstance().getApplicationInfo().verbosity) != Verbosity::SILENT ) 	\
	{																								\
		(base).getInstance().getApplicationInfo().elementLogFunc(base, severity, message);			\
	}

}

#include "ZuazoBase.inl"