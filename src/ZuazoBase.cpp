#include <zuazo/ZuazoBase.h>

#include <zuazo/Utils/Functions.h>

#include <future>

namespace Zuazo {

/*
 * ZuazoBase::Impl
 */

struct ZuazoBase::Impl {
	enum Moments {
		PRE_UPDATE,
		UPDATE,
		POST_UPDATE,

		COUNT
	};

	using UpdateCallbacks = std::array<UpdateCallback, COUNT>;

	Instance& 						instance;

	bool							opened;

	//Callbacks
	MoveCallback					moveCallback;
	OpenCallback					openCallback;
	AsyncOpenCallback				asyncOpenCallback;
	CloseCallback					closeCallback;
	AsyncCloseCallback				asyncCloseCallback;
	UpdateCallbacks					updateCallbacks;

	Impl(	Instance& instance, 
			MoveCallback moveCbk,
			OpenCallback openCbk,
			AsyncOpenCallback asyncOpenCbk,
			CloseCallback closeCbk,
			AsyncCloseCallback asyncCloseCbk,
			UpdateCallback updateCbk ) noexcept
		: instance(instance)
		, opened(false)
		, moveCallback(std::move(moveCbk))
		, openCallback(std::move(openCbk))
		, asyncOpenCallback(std::move(asyncOpenCbk))
		, closeCallback(std::move(closeCbk))
		, asyncCloseCallback(std::move(asyncCloseCbk))
		, updateCallbacks{ UpdateCallback(), std::move(updateCbk), UpdateCallback() }
	{
	}

	~Impl() {
		//Just in case upper class has not unsubscribed from updates
		disableRegularUpdate();
		disablePeriodicUpdate();
	}

	void moved(ZuazoBase& base) {
		Utils::invokeIf(moveCallback, base);
	}


	void open(ZuazoBase& base) {
		if(opened == false) {
			Utils::invokeIf(openCallback, base);
			opened = true;

			ZUAZO_BASE_LOG(base, Severity::verbose, "Opened");
		} else {
			ZUAZO_BASE_LOG(base, Severity::error, "Opening an already opened element");
		}

		assert(opened == true);
	}

	void asyncOpen(ZuazoBase& base, std::unique_lock<Instance>& lock) {
		assert(lock.owns_lock());

		if(opened == false) {
			if(asyncOpenCallback) {
				asyncOpenCallback(base, lock);
			} else {
				Utils::invokeIf(openCallback, base);
			}
			opened = true;

			ZUAZO_BASE_LOG(base, Severity::verbose, "Opened asynchronously");
		} else {
			ZUAZO_BASE_LOG(base, Severity::error, "Opening asynchronously an already opened element");
		}

		assert(opened == true);
		assert(lock.owns_lock());
	}

	void close(ZuazoBase& base) {
		if(opened == true) {
			Utils::invokeIf(closeCallback, base);
			opened = false;

			ZUAZO_BASE_LOG(base, Severity::verbose, "Closed");
		} else {
			ZUAZO_BASE_LOG(base, Severity::error, "Closing an already closed element");
		}

		assert(opened == false);
	}

	void asyncClose(ZuazoBase& base, std::unique_lock<Instance>& lock) {
		assert(lock.owns_lock());

		if(opened == true) {
			if(asyncCloseCallback) {
				asyncCloseCallback(base, lock);
			} else {
				Utils::invokeIf(closeCallback, base);
			}
			opened = false;

			ZUAZO_BASE_LOG(base, Severity::verbose, "Closed asynchronously");
		} else {
			ZUAZO_BASE_LOG(base, Severity::error, "Closing asynchronously an already opened element");
		}

		assert(opened == false);
		assert(lock.owns_lock());
	}

	bool isOpen() const noexcept {
		return opened;
	}



	Instance& getInstance() const noexcept {
		return instance;
	}



	void setPreUpdateCallback(UpdateCallback cbk) noexcept {
		updateCallbacks[PRE_UPDATE] = std::move(cbk);
	}

	const UpdateCallback& getPreUpdateCallback() const noexcept {
		return updateCallbacks[PRE_UPDATE];
	}

	void setUpdateCallback(UpdateCallback cbk) noexcept {
		updateCallbacks[UPDATE] = std::move(cbk);
	}

	const UpdateCallback& getUpdateCallback() const noexcept {
		return updateCallbacks[UPDATE];
	}

	void setPostUpdateCallback(UpdateCallback cbk) noexcept {
		updateCallbacks[POST_UPDATE] = std::move(cbk);
	}

	const UpdateCallback& getPostUpdateCallback() const noexcept {
		return updateCallbacks[POST_UPDATE];
	}



	void setMoveCallback(MoveCallback cbk) noexcept {
		moveCallback = std::move(cbk);
	}

	const MoveCallback& getMoveCallback() const noexcept {
		return moveCallback;
	}



	void setOpenCallback(OpenCallback cbk) noexcept {
		openCallback = std::move(cbk);
	}

	const OpenCallback& getOpenCallback() const noexcept {
		return openCallback;
	}

	void setCloseCallback(CloseCallback cbk) noexcept {
		closeCallback = std::move(cbk);
	}

	const CloseCallback& getCloseCallback() const noexcept {
		return closeCallback;
	}



	void update() const noexcept {
		for(const auto& cbk : updateCallbacks) {
			Utils::invokeIf(cbk);
		}
	}

	void enableRegularUpdate(Instance::Priority prior) const {
		for(const auto& cbk : updateCallbacks) {
			getInstance().addRegularCallback(cbk, prior);
		}
	}

	void disableRegularUpdate() const noexcept {
		for(const auto& cbk : updateCallbacks) {
			getInstance().removeRegularCallback(cbk);
		}
	}

	void enablePeriodicUpdate(	Instance::Priority prior, 
								Duration period) const 
	{
		for(const auto& cbk : updateCallbacks) {
			getInstance().addPeriodicCallback(cbk, prior, period);
		}
	}

	void disablePeriodicUpdate() const noexcept {
		for(const auto& cbk : updateCallbacks) {
			getInstance().removePeriodicCallback(cbk);
		}
	}

};



/*
 * ZuazoBase
 */

ZuazoBase::ZuazoBase(	Instance& instance, 
						std::string name,
						Utils::BufferView<const PadRef> pads,
						MoveCallback moveCbk,
						OpenCallback openCbk,
						AsyncOpenCallback asyncOpenCbk,
						CloseCallback closeCbk,
						AsyncCloseCallback asyncCloseCbk,
						UpdateCallback updateCbk )
	: Signal::Layout(std::move(name), pads.begin(), pads.end())
	, m_impl({}, instance, std::move(moveCbk), std::move(openCbk), std::move(asyncOpenCbk), std::move(closeCbk), std::move(asyncCloseCbk), std::move(updateCbk))
{
	ZUAZO_BASE_LOG(*this, Severity::verbose, "Constructed");
}

ZuazoBase::ZuazoBase(ZuazoBase&& other) noexcept
	: Signal::Layout(std::move(static_cast<Signal::Layout&>(other)))
	, m_impl(std::move(other.m_impl))
{
	if(m_impl) {
		m_impl->moved(*this);
		ZUAZO_BASE_LOG(*this, Severity::verbose, "Moved");
	}
}

ZuazoBase::~ZuazoBase() {
	if(m_impl) {
		ZUAZO_BASE_LOG(*this, Severity::verbose, "Destroyed");
	}
}


ZuazoBase& ZuazoBase::operator=(ZuazoBase&& other) noexcept {
	static_cast<Signal::Layout&>(*this) = std::move(static_cast<Signal::Layout&>(other));
	m_impl = std::move(other.m_impl);

	if(m_impl) {
		m_impl->moved(*this);
		ZUAZO_BASE_LOG(*this, Severity::verbose, "Move assigned");
	}

	return *this;
}



void ZuazoBase::open() {
	m_impl->open(*this);
}

void ZuazoBase::asyncOpen(std::unique_lock<Instance>& lock) {
	m_impl->asyncOpen(*this, lock);
}

void ZuazoBase::close() {
	m_impl->close(*this);
}

void ZuazoBase::asyncClose(std::unique_lock<Instance>& lock) {
	m_impl->asyncClose(*this, lock);
}

bool ZuazoBase::isOpen() const noexcept {
	return m_impl->isOpen();
}



Instance& ZuazoBase::getInstance() const noexcept {
	return m_impl->getInstance();
}



void ZuazoBase::setPreUpdateCallback(UpdateCallback cbk) noexcept {
	m_impl->setPreUpdateCallback(std::move(cbk));
}

const ZuazoBase::UpdateCallback& ZuazoBase::getPreUpdateCallback() const noexcept {
	return m_impl->getPreUpdateCallback();
}

void ZuazoBase::setUpdateCallback(UpdateCallback cbk) noexcept {
	m_impl->setUpdateCallback(std::move(cbk));
}

const ZuazoBase::UpdateCallback& ZuazoBase::getUpdateCallback() const noexcept {
	return m_impl->getUpdateCallback();
}

void ZuazoBase::setPostUpdateCallback(UpdateCallback cbk) noexcept {
	m_impl->setPostUpdateCallback(std::move(cbk));
}

const ZuazoBase::UpdateCallback& ZuazoBase::getPostUpdateCallback() const noexcept {
	return m_impl->getPostUpdateCallback();
}



void ZuazoBase::setMoveCallback(MoveCallback cbk) noexcept {
	m_impl->setMoveCallback(std::move(cbk));
}

const ZuazoBase::MoveCallback& ZuazoBase::getMoveCallback() const noexcept {
	return m_impl->getMoveCallback();
}



void ZuazoBase::setOpenCallback(OpenCallback cbk) noexcept {
	m_impl->setOpenCallback(std::move(cbk));
}

const ZuazoBase::OpenCallback& ZuazoBase::getOpenCallback() const noexcept {
	return m_impl->getOpenCallback();
}

void ZuazoBase::setCloseCallback(CloseCallback cbk) noexcept {
	m_impl->setCloseCallback(std::move(cbk));
}

const ZuazoBase::CloseCallback& ZuazoBase::getCloseCallback() const noexcept {
	return m_impl->getCloseCallback();
}



void ZuazoBase::update() const noexcept {
	m_impl->update();
}

void ZuazoBase::enableRegularUpdate(Instance::Priority prior) const {
	m_impl->enableRegularUpdate(prior);
}

void ZuazoBase::disableRegularUpdate() const noexcept {
	m_impl->disableRegularUpdate();
}

void ZuazoBase::enablePeriodicUpdate(	Instance::Priority prior, 
										Duration period) const 
{
	m_impl->enablePeriodicUpdate(prior, period);
}

void ZuazoBase::disablePeriodicUpdate() const noexcept {
	m_impl->disablePeriodicUpdate();
}

}