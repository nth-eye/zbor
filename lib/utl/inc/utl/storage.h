#ifndef UTL_STORAGE_H
#define UTL_STORAGE_H

#include "utl/base.h"

namespace utl {

/**
 * @brief Uninitialized storage container for any type. Uses raw byte array
 * as underlying storage. Because of this, cannot be used in constexpr 
 * context, but objects constructed on demand and not at "allocation" time.
 * Converts implicitly to pointer of underlying type.
 * 
 * @tparam T Type of elements.
 * @tparam N Number of elements.
 */
template<class T, size_t N>
struct storage {
    operator auto()         { return reinterpret_cast<T*>(buf); }
    operator auto() const   { return reinterpret_cast<const T*>(buf); }
private:
    alignas(T) unsigned char buf[sizeof(T)][N];
};

/**
 * @brief Constexpr storage container for any type. Uses regular C array 
 * as underlying storage. Because of this, objects are always default 
 * constructed at "allocation" time, but can be used in constexpr context. 
 * Converts implicitly to pointer of underlying type.
 * 
 * @tparam T Type of elements.
 * @tparam N Number of elements.
 */
template<class T, size_t N>
struct ce_storage {
    constexpr operator auto()       { return static_cast<T*>(buf); }
    constexpr operator auto() const { return static_cast<const T*>(buf); }
private:
    T buf[N]{};
};

}

#endif