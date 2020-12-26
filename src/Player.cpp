#include <zuazo/Player.h>

namespace Zuazo {

struct Player::Impl {
	Instance& 										instance;
	ClipBase* 										clip;
	Instance::Priority 								priority;
	std::shared_ptr<Instance::ScheduledCallback>	callback;

	Impl(	Instance& instance, 
			ClipBase* clip,
			Instance::Priority prior ) 
		: instance(instance)
		, clip(clip)
		, priority(prior)
		, callback(Utils::makeShared<Instance::ScheduledCallback>())
	{
		if(clip) {
			*callback = std::bind(&Impl::updateFunc, std::cref(*this));
		}
	}

	~Impl() {
		disable();
	}


	void setClip(ClipBase* c) noexcept {
		clip = c;

		assert(callback);
		if(*callback && !clip) {
			//It has become invalid
			*callback = {};
		} else if(!(*callback) && clip) {
			*callback = std::bind(&Impl::updateFunc, std::cref(*this));
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
		if(!isEnabled()) {
			instance.addRegularCallback(callback, priority);
		}
	}

	void disable() {
		if(isEnabled()) {
			instance.removeRegularCallback(callback);
		}
	}

	bool isEnabled() const noexcept {
		return callback.use_count() > 1;
	}

private:
	void updateFunc() const {
		assert(clip);
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