#include <zuazo/ZuazoBase.h>

namespace Zuazo {

ZuazoBase::ZuazoBase(	Instance& instance, 
						std::string name,
						OpenCallback openCbk,
						CloseCallback closeCbk,
						UpdateCallback updateCbk,
						std::initializer_list<PadRef> pads )
	: Signal::Layout(move(name), pads)
	, m_instance(instance)
	, m_isOpen(false)
	, m_openCallback(std::move(openCbk))
	, m_closeCallback(std::move(closeCbk))
	, m_updateCallbacks{std::make_shared<UpdateCallback>(),
						std::make_shared<UpdateCallback>(std::move(updateCbk)),
						std::make_shared<UpdateCallback>()}
{
	ZUAZO_BASE_LOG(*this, Severity::VERBOSE, "Constructed");
}

ZuazoBase::~ZuazoBase() {
	ZUAZO_BASE_LOG(*this, Severity::VERBOSE, "Destroyed");
}

void ZuazoBase::open() {
	if(m_isOpen == false) {
		if(m_openCallback) m_openCallback(*this);
		m_isOpen = true;
		ZUAZO_BASE_LOG(*this, Severity::VERBOSE, "Opened");
	} else {
		ZUAZO_BASE_LOG(*this, Severity::ERROR, "Opening an already opened element");
	}
}

void ZuazoBase::close() {
	if(m_isOpen == true) {
		if(m_closeCallback) m_closeCallback(*this);
		m_isOpen = false;
		ZUAZO_BASE_LOG(*this, Severity::VERBOSE, "Closed");
	} else {
		ZUAZO_BASE_LOG(*this, Severity::ERROR, "Closing an already closed element");
	}
}

bool ZuazoBase::isOpen() const {
	return m_isOpen;
}



Instance& ZuazoBase::getInstance() const {
	return m_instance;
}



void ZuazoBase::setPreUpdateCallback(UpdateCallback cbk) {
	*(m_updateCallbacks[PRE_UPDATE]) = std::move(cbk);
}

const ZuazoBase::UpdateCallback& ZuazoBase::setPreUpdateCallback() const {
	return *(m_updateCallbacks[PRE_UPDATE]);
}

void ZuazoBase::setUpdateCallback(UpdateCallback cbk) {
	*(m_updateCallbacks[UPDATE]) = std::move(cbk);
}

const ZuazoBase::UpdateCallback& ZuazoBase::setUpdateCallback() const {
	return *(m_updateCallbacks[UPDATE]) ;
}

void ZuazoBase::setPostUpdateCallback(UpdateCallback cbk) {
	*(m_updateCallbacks[POST_UPDATE]) = std::move(cbk);
}

const ZuazoBase::UpdateCallback& ZuazoBase::setPostUpdateCallback() const {
	return *(m_updateCallbacks[POST_UPDATE]);
}



void ZuazoBase::update() const {
	for(const auto& cbk : m_updateCallbacks) {
		(*cbk)();
	}
}

void ZuazoBase::enableRegularUpdate(Instance::Priority prior) const {
	for(const auto& cbk : m_updateCallbacks) {
		getInstance().addRegularCallback(cbk, prior);
	}
}

void ZuazoBase::disableRegularUpdate() const {
	for(const auto& cbk : m_updateCallbacks) {
		getInstance().removeRegularCallback(cbk);
	}
}

void ZuazoBase::enablePeriodicUpdate(	Instance::Priority prior, 
										Duration period) const 
{
	for(const auto& cbk : m_updateCallbacks) {
		getInstance().addPeriodicCallback(cbk, prior, period);
	}
}

void ZuazoBase::disablePeriodicUpdate() const {
	for(const auto& cbk : m_updateCallbacks) {
		getInstance().removePeriodicCallback(cbk);
	}
}

}