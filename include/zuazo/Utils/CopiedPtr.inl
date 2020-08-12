#include "CopiedPtr.h"

namespace Zuazo::Utils {    
/*
 * Cloner
 */

template<typename T>
inline T* DefaultCloner<T>::operator()(const T* ptr) const {
	return new T(*ptr);
}

template<typename T>
inline T* PolymorphicCloner<T>::operator()(const T* ptr) const {
	return ptr->clone();
}

/*
 * CopiedPtr
 */

template<typename T>
constexpr CopiedPtr<T>::CopiedPtr()
    : m_owner(nullptr)
    , m_pointer(nullptr)
{
}

template<typename T>
constexpr CopiedPtr<T>::CopiedPtr(std::nullptr_t)
    : CopiedPtr()
{
}

template<typename T>
template <typename U, typename D, typename C>
inline CopiedPtr<T>::CopiedPtr(std::unique_ptr<U, D> pointer, C cloner) 
    : m_owner(makeUnique<Owner<T, D, C>>(std::move(pointer), std::move(cloner)))
    , m_pointer(pointer)
{
}

template<typename T>
template <typename U, typename D, typename C>
inline CopiedPtr<T>::CopiedPtr(U* pointer, D deleter, C cloner)
    : CopiedPtr(std::unique_ptr<U, D>(pointer, deleter), cloner)
{
}

template<typename T>
template <typename... Args>
inline CopiedPtr<T>::CopiedPtr(std::in_place_t, Args&&... args)	
    : m_owner(makeUnique<CompactOwner>(std::forward<Args>(args)...))
    , m_pointer(m_owner->get())
{
}

template<typename T>
inline CopiedPtr<T>::CopiedPtr(const CopiedPtr& other)
    : m_owner(other ? other.m_owner->clone() : nullptr)
    , m_pointer(m_owner ? m_owner->get() : nullptr)
{
}

template<typename T>
template <typename U>
inline CopiedPtr<T>::CopiedPtr(const CopiedPtr<U>& other)
    : m_owner(other ? makeUnique<IndirectOwner<U>>(other.m_owner->clone()) : nullptr)
    , m_pointer(m_owner ? m_owner->get() : nullptr)
{
}

template<typename T>
inline CopiedPtr<T>::CopiedPtr(CopiedPtr&& other)
	: m_owner(other.m_owner)
	, m_pointer(other.m_ptr)
{
	other.m_owner = nullptr;
	other.m_ptr = nullptr;
}

template<typename T>
template <typename U>
inline CopiedPtr<T>::CopiedPtr(CopiedPtr<U>&& other)
    : m_owner(other ? makeUnique<IndirectOwner<U>>(other.m_owner) : nullptr)
    , m_pointer(m_owner ? m_owner->get() : nullptr)
{
	other.m_owner = nullptr;
	other.m_ptr = nullptr;
}


template<typename T>
template <typename U>
inline CopiedPtr<T>& CopiedPtr<T>::operator=(U* pointer) {
	CopiedPtr(pointer).swap(*this);
	return *this;
}

template<typename T>
inline CopiedPtr<T>& CopiedPtr<T>::operator=(std::nullptr_t) {
	CopiedPtr(nullptr).swap(*this);
	return *this;
}

template<typename T>
template <typename U, typename D>
inline CopiedPtr<T>& CopiedPtr<T>::operator=(std::unique_ptr<U, D> pointer) {
	CopiedPtr(std::move(pointer)).swap(*this);
	return *this;
}

template<typename T>
inline CopiedPtr<T>& CopiedPtr<T>::operator=(const CopiedPtr& other) {
	CopiedPtr(other).swap(*this);
	return *this;
}

template<typename T>
template <typename U>
inline CopiedPtr<T>& CopiedPtr<T>::operator=(const CopiedPtr<U>& other) {
	CopiedPtr(other).swap(*this);
	return *this;
}

template<typename T>
inline CopiedPtr<T>& CopiedPtr<T>::operator=(CopiedPtr&& other) {
	CopiedPtr(std::move(other)).swap(*this);
	return *this;
}

template<typename T>
template <typename U>
inline CopiedPtr<T>& CopiedPtr<T>::operator=(CopiedPtr<U>&& other) {
	CopiedPtr(std::move(other)).swap(*this);
	return *this;
}



template<typename T>
template<typename U>
inline bool CopiedPtr<T>::operator==(const CopiedPtr<U>& other) const {
	return m_pointer == other.m_pointer;
}

template<typename T>
template<typename U>
inline bool CopiedPtr<T>::operator!=(const CopiedPtr<U>& other) const {
	return m_pointer != other.m_pointer;
}

template<typename T>
template<typename U>
inline bool CopiedPtr<T>::operator<(const CopiedPtr<U>& other) const {
	return m_pointer < other.m_pointer;
}

template<typename T>
template<typename U>
inline bool CopiedPtr<T>::operator<=(const CopiedPtr<U>& other) const {
	return m_pointer <= other.m_pointer;
}

template<typename T>
template<typename U>
inline bool CopiedPtr<T>::operator>(const CopiedPtr<U>& other) const {
	return m_pointer > other.m_pointer;
}

template<typename T>
template<typename U>
inline bool CopiedPtr<T>::operator>=(const CopiedPtr<U>& other) const {
	return m_pointer >= other.m_pointer;
}



template<typename T>
inline CopiedPtr<T>::operator bool() const {
	return m_pointer != nullptr;
}

template<typename T>
inline typename CopiedPtr<T>::reference CopiedPtr<T>::operator*() const {
	assert(m_pointer);
	return *m_pointer;
}

template<typename T>
inline typename CopiedPtr<T>::pointer CopiedPtr<T>::operator->() const {
	assert(m_pointer);
	return m_pointer;
}


template<typename T>
inline void CopiedPtr<T>::reset() {
	CopiedPtr().swap(*this);
}

template<typename T>
template <typename U, typename D, typename C>
inline void CopiedPtr<T>::reset(std::unique_ptr<U, D> pointer, C cloner) {
	CopiedPtr(std::move(pointer), std::move(cloner)).swap(*this);
}

template<typename T>
template <typename U, typename D, typename C>
inline void CopiedPtr<T>::reset(U* pointer, D deleter, C cloner) {
	CopiedPtr(pointer, cloner, deleter).swap(*this);
}


template<typename T>
inline typename CopiedPtr<T>::pointer CopiedPtr<T>::get() const {
	return m_pointer;
}


template<typename T>
inline void CopiedPtr<T>::swap(CopiedPtr& other) {
	std::swap(m_owner, other.m_owner);
	std::swap(m_pointer, other.m_pointer);
}


/*
 *  CopiedPtr::Owner
 */

template<typename T>
template <typename U, typename D, typename C>
inline CopiedPtr<T>::Owner<U, D, C>::Owner(std::unique_ptr<U, D> owned, C cloner)
	: m_owned(std::move(owned))
	, m_cloner(std::move(cloner))
{
}

template<typename T>
template <typename U, typename D, typename C>
inline typename CopiedPtr<T>::pointer
CopiedPtr<T>::Owner<U, D, C>::get() const {
	return m_pointer;
}

template<typename T>
template <typename U, typename D, typename C>
inline typename CopiedPtr<T>::OwnerPtr
CopiedPtr<T>::Owner<U, D, C>::clone() const {
	return makeUnique<Owner>(
        std::unique_ptr<U, D>(m_cloner(m_pointer), m_pointer.get_deleter()), 
        m_cloner
    );
}

/*
 *  CopiedPtr::CompactOwner
 */

template<typename T>
template <typename... Args>
inline CopiedPtr<T>::CompactOwner::CompactOwner(Args&&... args)
	: m_object(std::forward<Args>(args)...)
{
}

template<typename T>
inline typename CopiedPtr<T>::pointer
CopiedPtr<T>::CompactOwner::get() const {
	return &m_object;
}

template<typename T>
inline typename CopiedPtr<T>::OwnerPtr
CopiedPtr<T>::CompactOwner::clone() const {
	return makeUnique<CompactOwner>(m_object);
}

/*
 *  CopiedPtr::IndirectOwner
 */

template<typename T>
template <typename U>
inline CopiedPtr<T>::IndirectOwner<U>::IndirectOwner(IndirectPtr indirect)
	: m_indirection(std::move(indirect))
{
}

template<typename T>
template <typename U>
inline typename CopiedPtr<T>::pointer
CopiedPtr<T>::IndirectOwner<U>::IndirectOwner::get() const {
	return m_indirection->get();
}

template<typename T>
template <typename U>
inline typename CopiedPtr<T>::OwnerPtr
CopiedPtr<T>::IndirectOwner<U>::IndirectOwner::clone() const {
	return makeUnique<IndirectOwner<U>>(
        m_indirection->clone()
    );
}


/*
 * makeCopied
 */

template<typename T, typename... Args>
inline typename std::enable_if<!std::is_array<T>::value, CopiedPtr<T>>::type
makeCopied(Args&&... args) {
    return CopiedPtr<T>(std::in_place, std::forward<Args>(args)...);
}

}