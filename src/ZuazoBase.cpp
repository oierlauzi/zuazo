#include <zuazo/ZuazoBase.h>

namespace Zuazo {

ZuazoBase::ZuazoBase(	Instance& instance, 
						std::string name,
						std::initializer_list<PadRef> pads,
						MoveCallback moveCbk,
						OpenCallback openCbk,
						CloseCallback closeCbk,
						UpdateCallback updateCbk )
	: Signal::Layout(std::move(name), pads)
	, m_instance(instance)
	, m_isOpen(false)
	, m_moveCallback(std::move(moveCbk))
	, m_openCallback(std::move(openCbk))
	, m_closeCallback(std::move(closeCbk))
	, m_updateCallbacks{std::make_shared<UpdateCallback>(),
						std::make_shared<UpdateCallback>(std::move(updateCbk)),
						std::make_shared<UpdateCallback>()}
{
	ZUAZO_BASE_LOG(*this, Severity::VERBOSE, "Constructed");
}

ZuazoBase::ZuazoBase(ZuazoBase&& other)
	: Signal::Layout(std::move(static_cast<Signal::Layout&>(other)))
	, m_instance(other.m_instance)
	, m_isOpen(other.m_isOpen)
	, m_moveCallback(std::move(other.m_moveCallback))
	, m_openCallback(std::move(other.m_openCallback))
	, m_closeCallback(std::move(other.m_closeCallback))
	, m_updateCallbacks(std::move(other.m_updateCallbacks))
{
	m_moveCallback(*this);
	ZUAZO_BASE_LOG(*this, Severity::VERBOSE, "Moved");
}

ZuazoBase::~ZuazoBase() {
	//Just in case upper class has not desuscribed from updates
	disableRegularUpdate();
	disablePeriodicUpdate();
	
	ZUAZO_BASE_LOG(*this, Severity::VERBOSE, "Destroyed");
}

ZuazoBase& ZuazoBase::operator=(ZuazoBase&& other) {
	static_cast<Signal::Layout&>(*this) = std::move(static_cast<Signal::Layout&>(other));
	m_instance = other.m_instance;
	m_isOpen = other.m_isOpen;
	m_moveCallback = std::move(other.m_moveCallback);
	m_openCallback = std::move(other.m_openCallback);
	m_closeCallback = std::move(other.m_closeCallback);
	m_updateCallbacks = std::move(other.m_updateCallbacks);

	m_moveCallback(*this);
	ZUAZO_BASE_LOG(*this, Severity::VERBOSE, "Move assigned");

	return *this;
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

const ZuazoBase::UpdateCallback& ZuazoBase::getPreUpdateCallback() const {
	return *(m_updateCallbacks[PRE_UPDATE]);
}

void ZuazoBase::setUpdateCallback(UpdateCallback cbk) {
	*(m_updateCallbacks[UPDATE]) = std::move(cbk);
}

const ZuazoBase::UpdateCallback& ZuazoBase::getUpdateCallback() const {
	return *(m_updateCallbacks[UPDATE]) ;
}

void ZuazoBase::setPostUpdateCallback(UpdateCallback cbk) {
	*(m_updateCallbacks[POST_UPDATE]) = std::move(cbk);
}

const ZuazoBase::UpdateCallback& ZuazoBase::getPostUpdateCallback() const {
	return *(m_updateCallbacks[POST_UPDATE]);
}



void ZuazoBase::setMoveCallback(MoveCallback cbk) {
	m_moveCallback = std::move(cbk);
}

const ZuazoBase::MoveCallback& ZuazoBase::getMoveCallback() const {
	return m_moveCallback;
}



void ZuazoBase::setOpenCallback(OpenCallback cbk) {
	m_openCallback = std::move(cbk);
}

const ZuazoBase::OpenCallback& ZuazoBase::getOpenCallback() const {
	return m_openCallback;
}

void ZuazoBase::setCloseCallback(CloseCallback cbk) {
	m_closeCallback = std::move(cbk);
}

const ZuazoBase::CloseCallback& ZuazoBase::getCloseCallback() const {
	return m_closeCallback;
}



void ZuazoBase::update() const {
	for(const auto& cbk : m_updateCallbacks) {
		assert(cbk);
		if(*cbk) (*cbk)();
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