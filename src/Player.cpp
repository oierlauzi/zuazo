#include <zuazo/Player.h>

namespace Zuazo {

struct Player::Impl {
	Instance& 										instance;
	ClipBase* 										clip;
	Instance::Priority 								priority;
	Instance::ScheduledCallback						callback;
	bool											enabled;

	Impl(	Instance& instance, 
			ClipBase* clip,
			Instance::Priority prior ) 
		: instance(instance)
		, clip(clip)
		, priority(prior)
		, callback()
		, enabled(false)
	{
		if(clip) {
			callback = std::bind(&Impl::updateFunc, std::cref(*this));
		}
	}

	~Impl() {
		disable();
	}


	void setClip(ClipBase* c) noexcept {
		clip = c;

		if(callback && !clip) {
			//It has become invalid
			callback = {};
		} else if(!callback && clip) {
			callback = std::bind(&Impl::updateFunc, std::cref(*this));
		}
	}

	ClipBase* getClip() const noexcept {
		return clip;
	}


	void setPriority(Instance::Priority prior) noexcept {
		priority = prior;

		if(isEnabled()) {
			instance.removeRegularCallback(callback);
			instance.addRegularCallback(callback, priority);
		}
	}

	Instance::Priority getPriority() const noexcept {
		return priority;
	}


	void enable() {
		if(!enabled) {
			instance.addRegularCallback(callback, priority);
			enabled = true;
		}

		assert(enabled == true);
	}

	void disable() {
		if(enabled) {
			instance.removeRegularCallback(callback);
			enabled = false;
		}

		assert(enabled == false);
	}

	bool isEnabled() const noexcept {
		return enabled;
	}

private:
	void updateFunc() const {
		assert(clip); //Only called if a clip was set
		clip->advance(instance.getDeltaT());
	}

};



Player::Player(	Instance& instance, 
				ClipBase* clip,
				Instance::Priority priority )
	: m_impl({}, instance, clip, priority)
{
}

Player::Player(Player&& other) = default;

Player::~Player() = default;

Player& Player::operator=(Player&& other) = default;


void Player::setClip(ClipBase* clip) noexcept {
	m_impl->setClip(clip);
}

ClipBase* Player::getClip() const noexcept {
	return m_impl->getClip();
}


void Player::setPriority(Instance::Priority prior) noexcept {
	m_impl->setPriority(prior);
}

Instance::Priority Player::getPriority() const noexcept {
	return m_impl->getPriority();
}


void Player::enable() {
	m_impl->enable();
}

void Player::disable() {
	m_impl->disable();
}

bool Player::isEnabled() const noexcept {
	return m_impl->isEnabled();
}

}