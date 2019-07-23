template <size_t aligment = 1>
constexpr size_t defualt_hash_func(const void* ptr, size_t size){
    static_assert(aligment > 0 && aligment <= alignof(hash_t), "Invalid aligment");
    typedef typename boost::uint_t<aligment * 8>::least AlignedType;

    size_t result = 0;

    for(const AlignedType* i = ptr; i < ptr + size; ++i){
        result = hashCombine(
            result, 
            static_cast<size_t>(*i)
        );
    }

    return result;
}

template<typename T>
constexpr size_t defualt_hash_func(const T& x){
    if constexpr(std::is_empty<T>::value){
        return 0;
    } else {
        constexpr size_t aligment = Math::gcd(alignof(T), alignof(hash_t));
        return defualt_hash_func<aligment>(&x, sizeof(T));
    }
}