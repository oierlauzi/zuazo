#pragma once

#include "Functions.h"

#include <memory>
#include <utility>
#include <type_traits>

namespace Zuazo::Utils {

/*
 * Based on Aurora:
 * https://github.com/Bromeon/Aurora/blob/master/include/Aurora/SmartPtr/CopiedPtr.hpp
 * https://github.com/Bromeon/Aurora/blob/master/include/Aurora/SmartPtr/Detail/PtrOwner.hpp
 */

template<typename T>
using DefaultDeleter = std::default_delete<T>;

template<typename T>
struct DefaultCloner {
    T* operator()(const T* ptr) const;
};

template<typename T>
struct PolymorphicCloner {
    T* operator()(const T* ptr) const;
};


template<typename T>
class CopiedPtr {
public:
	using element_type = T;
    using pointer = typename std::add_pointer<element_type>::type;
	using reference = typename std::add_lvalue_reference<element_type>::type;

	constexpr CopiedPtr();
	constexpr CopiedPtr(std::nullptr_t);

	template <typename U, typename D, typename C>
	explicit CopiedPtr(std::unique_ptr<U, D> pointer, C cloner = DefaultCloner<T>());
	template <typename U, typename D, typename C>
	explicit CopiedPtr(U* pointer, D deleter =  DefaultDeleter<T>(), C cloner = DefaultCloner<T>());

	CopiedPtr(const CopiedPtr& other);
	template <typename U>
	CopiedPtr(const CopiedPtr<U>& other);

	CopiedPtr(CopiedPtr&& other);
	template <typename U>
	CopiedPtr(CopiedPtr<U>&& other);

	template <typename... Args>
	explicit CopiedPtr(std::in_place_t, Args&&... args);

	~CopiedPtr() = default;

	template <typename U>
	CopiedPtr&					operator=(U* pointer);
	CopiedPtr&					operator=(std::nullptr_t);
	template <typename U, typename D>
	CopiedPtr<T>& 				operator=(std::unique_ptr<U, D> pointer);
	CopiedPtr& 					operator=(const CopiedPtr& other);
	template <typename U>
	CopiedPtr& 					operator=(const CopiedPtr<U>& other);

	CopiedPtr& 					operator=(CopiedPtr&& other);
	template <typename U>
	CopiedPtr&					operator=(CopiedPtr<U>&& other);

	operator bool() const;
	template <typename U>
	bool						operator==(const CopiedPtr<U>& other) const;	
	template <typename U>
	bool						operator!=(const CopiedPtr<U>& other) const;
	template <typename U>
	bool						operator<(const CopiedPtr<U>& other) const;
	template <typename U>
	bool						operator<=(const CopiedPtr<U>& other) const;
	template <typename U>
	bool						operator>(const CopiedPtr<U>& other) const;
	template <typename U>
	bool						operator>=(const CopiedPtr<U>& other) const;

	pointer 					operator-> () const;
	reference 					operator* () const;

	void 						reset();
	template <typename U, typename D, typename C>
	void 						reset(std::unique_ptr<U, D> pointer, C cloner = DefaultCloner<T>());
	template <typename U, typename D, typename C>
	void 						reset(U* pointer, D deleter =  DefaultDeleter<T>(), C cloner = DefaultCloner<T>());

	pointer 					get() const;
		
	void						swap(CopiedPtr& other);



private:
	struct OwnerBase;
	using OwnerPtr = std::unique_ptr<OwnerBase>;

	struct OwnerBase {
        virtual ~OwnerBase() = default;
		virtual OwnerPtr			clone() const = 0;
		virtual pointer				get() const = 0;
    };

    template <typename U, typename D, typename C>
	struct Owner final 
        : public OwnerBase
    {
		Owner(std::unique_ptr<U, D> owned, C cloner);
		virtual ~Owner() = default;

		virtual OwnerPtr			clone() const;
		virtual pointer				get() const;

    private:
		std::unique_ptr<U, D>		m_owned;
		C                           m_cloner;
	};

    struct CompactOwner final 
        : public OwnerBase 
    {
		template <typename... Args>
		CompactOwner(Args&&... args);

		virtual OwnerPtr			clone() const;
		virtual pointer				get() const;

    private:
		mutable T                   m_object;
    
	};

	template <typename U>
    struct IndirectOwner final 
        : public OwnerBase 
    {
		using IndirectPtr = typename CopiedPtr<U>::OwnerPtr;

		IndirectOwner(IndirectPtr indirection);
		virtual ~IndirectOwner() = default;

		virtual OwnerPtr			clone() const;
		virtual pointer				get() const;

    private:
		IndirectPtr					m_indirection;
    
	};

	OwnerPtr					m_owner;
	pointer						m_pointer;
};


template<typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, std::shared_ptr<T>>::type
makeCopied(Args&&... args);

//TODO makeCopied for unbounded array

}
#include "CopiedPtr.inl"