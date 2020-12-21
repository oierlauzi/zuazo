#pragma once

#include "Chrono.h"
#include "ClipBase.h"

#include <functional>
#include <map>

namespace Zuazo {

template <typename T>
class Animation 
	: public ClipBase
{
public:
	using AnimatedType = T;
	using Callback = std::function<void(const AnimatedType&)>;
	using KeyFrameMap = std::map<TimePoint, AnimatedType>;

	using iterator = typename KeyFrameMap::iterator;
	using const_iterator = typename KeyFrameMap::const_iterator;
	using reverse_iterator = typename KeyFrameMap::reverse_iterator;
	using const_reverse_iterator = typename KeyFrameMap::const_reverse_iterator;

	Animation(Duration duration = {}, Callback callback = {}) noexcept;
	Animation(const Animation& other) = default;
	Animation(Animation&& other) noexcept = default;
	~Animation() = default;

	Animation&							operator=(const Animation& other) = default;
	Animation&							operator=(Animation&& other) noexcept = default;

	using ClipBase::setDuration;

	void								setCallback(Callback cbk) noexcept;
	const Callback&						getCallback() const noexcept;

	void								refresh();
	void								sync() const;
	const AnimatedType&					getValue() const noexcept;


	iterator							begin() noexcept;
	const_iterator						begin() const noexcept;
	const_iterator						cbegin() const noexcept;

	iterator							end() noexcept;
	const_iterator						end() const noexcept;
	const_iterator						cend() const noexcept;

	reverse_iterator					rbegin() noexcept;
	const_reverse_iterator				rbegin() const noexcept;
	const_reverse_iterator				crbegin() const noexcept;

	reverse_iterator					rend() noexcept;
	const_reverse_iterator				rend() const noexcept;
	const_reverse_iterator				crend() const noexcept;

	bool								empty() const noexcept;
	size_t								size() const noexcept;

	void								clear() noexcept;
	std::pair<iterator, bool>			insertKeyFrame(TimePoint time, AnimatedType value);
	iterator							eraseKeyFrame(const_iterator ite);
	iterator							eraseKeyFrame(const_iterator first, const_iterator last);

	iterator							findKeyFrame(TimePoint time);
	const_iterator						findKeyFrame(TimePoint time) const;

	iterator							lowerBoundKeyFrame(TimePoint time);
	const_iterator						lowerBoundKeyFrame(TimePoint time) const;

	iterator							upperBoundKeyFrame(TimePoint time);
	const_iterator						upperBoundKeyFrame(TimePoint time) const;

private:
	Callback							m_callback;
	KeyFrameMap							m_keyFrames;
	AnimatedType						m_value;

	static void							refreshCallback(ClipBase& base);
};

}

#include "Animation.inl"