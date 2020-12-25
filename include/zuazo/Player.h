#pragma once

#include "Instance.h"
#include "ClipBase.h"
#include "Utils/Pimpl.h"

namespace Zuazo {

class Player {
public:
	Player(	Instance& instance, 
			ClipBase* clip = nullptr,
			Instance::Priority priority = Instance::PLAYER_PRIORITY);
	Player(const Player& other) = delete;
	Player(Player&& other);
	~Player();

	Player&					operator=(const Player& other) = delete;
	Player&					operator=(Player&& other);

	void					setClip(ClipBase* clip) noexcept;
	ClipBase*				getClip() const noexcept;

	void					setPriority(Instance::Priority prior) noexcept;
	Instance::Priority		getPriority() const noexcept;

	void					enable();
	void					disable();
	bool					isEnabled() const noexcept;

private:
	struct Impl;
	Utils::Pimpl<Impl>		m_impl;

};

}