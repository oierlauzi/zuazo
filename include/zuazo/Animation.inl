#include "Animation.h"

#include "Math/Interpolation.h"
#include "Utils/Functions.h"

namespace Zuazo {

template <typename T>
inline Animation<T>::Animation(Duration duration, Callback callback) noexcept
	: ClipBase(duration, {}, &Animation::refreshCallback)
	, m_callback(std::move(callback))
	, m_keyFrames()
{
}

template <typename T>
inline void Animation<T>::setCallback(Callback cbk) noexcept {
	m_callback = std::move(cbk);
}

template <typename T>
inline const typename Animation<T>::Callback& Animation<T>::getCallback() const noexcept {
	return m_callback;
}


template <typename T>
inline void Animation<T>::refresh() {
	const auto time = getTime();

	//Find the keyframe right after the current time
	const auto nextKeyframe = lowerBound(time);

	//Find the keyframe right before the current time
	const auto prevKeyframe = (nextKeyframe != cbegin()) ? std::prev(nextKeyframe) : cend();

	if(prevKeyframe != cend() && nextKeyframe != cend()) {
		//There is a keyframe before and after. Interpolate between them
		assert(prevKeyframe->first < time && time <= nextKeyframe->first);
		
		//Interpolate based on the progress
		const auto t = Math::ilerp<double>(
			prevKeyframe->first.time_since_epoch().count(), 
			nextKeyframe->first.time_since_epoch().count(),
			time.time_since_epoch().count()
		);
		const auto interpolated = static_cast<AnimatedType>(
			prevKeyframe->second * (1-t) + nextKeyframe->second * t
		);

		if(m_value != interpolated) {
			m_value = interpolated;
			sync();
		}

	} else if(prevKeyframe != cend()) {
		//There is only a keyframe before.
		assert(prevKeyframe->first < time);

		if(m_value != prevKeyframe->second) {
			m_value = prevKeyframe->second;
			sync();
		}

	} else if(nextKeyframe != cend()) {
		//There is only a keyframe after.
		assert(time <= nextKeyframe->first);

		if(m_value != nextKeyframe->second) {
			m_value = nextKeyframe->second;
			sync();
		}

	} //else no data
}

template <typename T>
inline void Animation<T>::sync() const {
	Utils::invokeIf(m_callback, m_value);
}


template <typename T>
inline const typename Animation<T>::AnimatedType& Animation<T>::getValue() const noexcept {
	return m_value;
}



template <typename T>
inline typename Animation<T>::iterator Animation<T>::begin() noexcept {
	return m_keyFrames.begin();
}

template <typename T>
inline typename Animation<T>::const_iterator Animation<T>::begin() const noexcept {
	return m_keyFrames.begin();
}

template <typename T>
inline typename Animation<T>::const_iterator Animation<T>::cbegin() const noexcept {
	return m_keyFrames.cbegin();
}


template <typename T>
inline typename Animation<T>::iterator Animation<T>::end() noexcept {
	return m_keyFrames.end();
}

template <typename T>
inline typename Animation<T>::const_iterator Animation<T>::end() const noexcept {
	return m_keyFrames.end();
}

template <typename T>
inline typename Animation<T>::const_iterator Animation<T>::cend() const noexcept {
	return m_keyFrames.cend();
}


template <typename T>
inline typename Animation<T>::reverse_iterator Animation<T>::rbegin() noexcept {
	return m_keyFrames.rbegin();
}

template <typename T>
inline typename Animation<T>::const_reverse_iterator Animation<T>::rbegin() const noexcept {
	return m_keyFrames.rbegin();
}

template <typename T>
inline typename Animation<T>::const_reverse_iterator Animation<T>::crbegin() const noexcept {
	return m_keyFrames.crbegin();
}


template <typename T>
inline typename Animation<T>::reverse_iterator Animation<T>::rend() noexcept {
	return m_keyFrames.rend();
}

template <typename T>
inline typename Animation<T>::const_reverse_iterator Animation<T>::rend() const noexcept {
	return m_keyFrames.rend();
}

template <typename T>
inline typename Animation<T>::const_reverse_iterator Animation<T>::crend() const noexcept {
	return m_keyFrames.crend();
}


template <typename T>
inline bool Animation<T>::empty() const noexcept {
	return m_keyFrames.empty();
}

template <typename T>
inline size_t Animation<T>::size() const noexcept {
	return m_keyFrames.size();
}


template <typename T>
inline void Animation<T>::clear() noexcept {
	m_keyFrames.clear();
}

template <typename T>
inline std::pair<typename Animation<T>::iterator, bool> Animation<T>::insert(TimePoint time, AnimatedType value) {
	return m_keyFrames.emplace(time, std::move(value));
}

template <typename T>
inline typename Animation<T>::iterator Animation<T>::erase(const_iterator ite) {
	return m_keyFrames.erase(ite);
}

template <typename T>
inline typename Animation<T>::iterator Animation<T>::erase(const_iterator first, const_iterator last) {
	return m_keyFrames.erase(first, last);
}


template <typename T>
inline typename Animation<T>::iterator Animation<T>::find(TimePoint time) {
	return m_keyFrames.find(time);
}

template <typename T>
inline typename Animation<T>::const_iterator Animation<T>::find(TimePoint time) const {
	return m_keyFrames.find(time);
}


template <typename T>
inline typename Animation<T>::iterator Animation<T>::lowerBound(TimePoint time) {
	return m_keyFrames.lower_bound(time);
}

template <typename T>
inline typename Animation<T>::const_iterator Animation<T>::lowerBound(TimePoint time) const {
	return m_keyFrames.lower_bound(time);
}


template <typename T>
inline typename Animation<T>::iterator Animation<T>::upperBound(TimePoint time) {
	return m_keyFrames.upper_bound(time);
}

template <typename T>
inline typename Animation<T>::const_iterator Animation<T>::upperBound(TimePoint time) const {
	return m_keyFrames.upper_bound(time);
}



template <typename T>
inline void Animation<T>::refreshCallback(ClipBase& base) {
	static_cast<Animation<T>&>(base).refresh();
}
	
}