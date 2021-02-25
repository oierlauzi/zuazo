#include "BezierLoop.h"

#include "Periodic.h"

#include <algorithm>

namespace Zuazo::Math {

template<typename T, size_t Deg>
inline BezierLoop<T, Deg>::BezierLoop(Utils::BufferView<const segment_data> data) 
	: m_values(
		reinterpret_cast<const value_type*>(data.cbegin()), 
		reinterpret_cast<const value_type*>(data.cend())
	)		
{
	assert(m_values.size()%degree() == 0);

	if(m_values.size()) {
		//Repeat the first element at the back, so that it loops
		//over to the first value when casting to Bezier
		m_values.push_back(m_values.front());
	}

	//This value is not possible!
	assert(m_values.size() != 1);
}

template<typename T, size_t Deg>
inline BezierLoop<T, Deg>::BezierLoop(Utils::BufferView<const bezier_type> segments) 
	: m_values(segments.size() > 0 ? segments.size()*degree() + 1 : 0)
{
	//This value is not possible!
	assert(m_values.size() != 1);

	//Fill the array
	for(size_t i = 0; i < segments.degree(); ++i) {
		setBezier(i, segments[i]);
	}
}


template<typename T, size_t Deg>
inline void BezierLoop<T, Deg>::setSegmentCount(size_t count) {
	m_values.resize(count ? count*degree() + 1 : 0);
	m_values.back() = m_values.front();

	assert(size()%degree()==0);
	assert(m_values.front() == m_values.back());
}

template<typename T, size_t Deg>
inline size_t BezierLoop<T, Deg>::getSegmentCount() const noexcept {
	assert(size()%degree()==0);
	return size() / degree();
}

template<typename T, size_t Deg>
inline void BezierLoop<T, Deg>::setSegment(size_t i, const bezier_type& s) {
	assert(i < getSegmentCount());
	for(size_t j = 0; j < s.size(); ++j) {
		setPoint((i*degree() + j) % size(), s[j]);
	}
}

template<typename T, size_t Deg>
inline const typename BezierLoop<T, Deg>::bezier_type& BezierLoop<T, Deg>::getSegment(size_t i) const noexcept {
	assert(i < getSegmentCount());
	return reinterpret_cast<const bezier_type&>(getPoint(i*degree()));
}


template<typename T, size_t Deg>
inline void BezierLoop<T, Deg>::setPoint(size_t i, const value_type& v) {
	assert(i < size());
	m_values[i] = v;

	//Ensure continuity
	if(i == 0) {
		m_values.back() = m_values.front();
	}
}

template<typename T, size_t Deg>
inline const typename BezierLoop<T, Deg>::value_type& BezierLoop<T, Deg>::getPoint(size_t i) const noexcept {
	assert(i < size());
	return m_values[i];
}


template<typename T, size_t Deg>
template<typename Q>
inline typename BezierLoop<T, Deg>::value_type BezierLoop<T, Deg>::operator()(Q t) const {
	//Sanitize the values
	const auto index = static_cast<intmax_t>(floor(t)); //Floors
	t -= index; //Obtain only the fractional part: [0, 1)
	const auto i = static_cast<size_t>(sawtooth(static_cast<intmax_t>(getSegmentCount()), index)); //Index in [0, segmentCount)

	//Ensure that the values are correct
	assert(Q(0) <= t && t < Q(1));
	assert(0U <= i && i < getSegmentCount());

	//Sample
	return getSegment(i)(t);
}


template<typename T, size_t Deg>
inline const typename BezierLoop<T, Deg>::value_type* BezierLoop<T, Deg>::data() const noexcept {
	return m_values.data();
}

template<typename T, size_t Deg>
inline size_t BezierLoop<T, Deg>::size() const noexcept {
	return m_values.size() > 1 ? m_values.size()-1 : 0;
}


template<typename T, size_t Deg>
inline const typename BezierLoop<T, Deg>::value_type* BezierLoop<T, Deg>::begin() const noexcept {
	return data();
}

template<typename T, size_t Deg>
inline const typename BezierLoop<T, Deg>::value_type* BezierLoop<T, Deg>::cbegin() const noexcept {
	return begin();
}

template<typename T, size_t Deg>
inline const typename BezierLoop<T, Deg>::value_type* BezierLoop<T, Deg>::end() const noexcept {
	return begin() + size();
}

template<typename T, size_t Deg>
inline const typename BezierLoop<T, Deg>::value_type* BezierLoop<T, Deg>::cend() const noexcept {
	return end();
}



template<typename T, size_t Deg>
inline void BezierLoop<T, Deg>::clear() noexcept {
	m_values.clear();
}

template<typename T, size_t Deg>
void BezierLoop<T, Deg>::reverse() noexcept {
	//Skip first and last elements
	std::reverse(
		std::next(m_values.begin()),
		std::prev(m_values.end())
	);
}


template<typename T, size_t Deg>
inline void BezierLoop<T, Deg>::lineTo(const value_type& value) {
	bezier_type bezier;
	std::fill(bezier.begin(), bezier.end(), value);
	pushSegment(bezier);
}

template<typename T, size_t Deg>
inline void BezierLoop<T, Deg>::pushSegment(const bezier_type& bezier) {
	if(m_values.empty()) {
		m_values.push_back(bezier.front());
	}

	assert(!m_values.empty());
	m_values.insert(
		std::prev(m_values.cend()), 
		bezier.cbegin(), 
		std::prev(bezier.cend())
	);

	assert(size()%degree()==0);
	assert(m_values.front() == m_values.back());
}



template<typename T, size_t Deg>
constexpr size_t BezierLoop<T, Deg>::degree() noexcept {
	return Deg;
}


template<typename T, size_t Deg>
inline bool operator==(const BezierLoop<T, Deg>& lhs, const BezierLoop<T, Deg>& rhs) noexcept {
	return std::equal(
		lhs.cbegin(), lhs.cend(),
		rhs.cbegin(), rhs.cend()
	);
}

template<typename T, size_t Deg>
inline bool operator!=(const BezierLoop<T, Deg>& lhs, const BezierLoop<T, Deg>& rhs) noexcept {
	return !operator==(lhs, rhs);
}


template<typename T, size_t Deg>
inline Utils::Range<typename BezierLoop<T, Deg>::value_type> getBoundaries(const BezierLoop<T, Deg>& s) {
	Utils::Range<typename BezierLoop<T, Deg>::value_type> result;

	if(s.getSegmentCount() > 0) {
		//Populate the result with the first segment
		result = getBoundaries(s.getSegment(0));

		//Accumulate the rest of the boundaries
		for(size_t i = 1; i < s.getSegmentCount(); ++i) {
			const auto newBoundaries = getBoundaries(s.getSegment(i));

			result = Utils::Range<typename BezierLoop<T, Deg>::value_type>(
				min(newBoundaries.getMin(), result.getMin()),
				max(newBoundaries.getMax(), result.getMax())
			);
		}
	}

	return result;
}

}