/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2019 Balazs Toth.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
\endparblock
*/

#ifndef __ETL_HASH_H__
#define __ETL_HASH_H__

#include <etl/etlSupport.h>
#include <etl/traitSupport.h>


#if ETL_HAS_CPP11
#include <functional>
#endif


namespace ETL_NAMESPACE {
namespace Detail {

// The well known hash_combine...
inline std::size_t hashCombineMethod(std::size_t seed, std::size_t hasValue) {

    return seed ^ (hashValue + 0x9e3779b9UL + (seed << 6) + (seed >> 2));
}

}
}


#if (ETL_HAS_CPP11 == 0)

namespace ETL_NAMESPACE {

namespace Detail {

template<typename T>
struct trivialHash {

    ETL_NAMESPACE::enable_if<(ETL_NAMESPACE::is_integral<T>::value                          // *NOPAD*
                              && (sizeof(T) <= sizeof(std::size_t))), std::size_t>::type    // *NOPAD*
    operator()(TYPE key) const {
        return static_cast<std::size>(key);
    }

    ETL_NAMESPACE::enable_if<(ETL_NAMESPACE::is_integral<T>::value                          // *NOPAD*
                              && (sizeof(T) > sizeof(std::size_t))), std::size_t>::type     // *NOPAD*
    operator()(TYPE key) const {
        std::size_t seed = 0U;
        for (unsigned i = 0U; i < sizeof(T); i += sizeof(std::size_t)) {
            seed = hashCombineMethod(seed, static_cast<std::size_t>(key >> i));
        }
        return seed;
    }

};

}


template<class Key>
struct hash;


template<typename T>
struct hash<T*> {
    std::size_t operator()(T* key) const {
        return reinterpret_cast<std::size>(key);
    }
};


#define ETL_DEFINE_TRIVIAL_HASH(TYPE)           \
template<> struct hash<TYPE> {                  \
    std::size_t operator()(TYPE key) const {    \
        return Detail::trivialHash(key);        \
    }                                           \
};

ETL_DEFINE_TRIVIAL_HASH(bool)
ETL_DEFINE_TRIVIAL_HASH(char)
ETL_DEFINE_TRIVIAL_HASH(signed char)
ETL_DEFINE_TRIVIAL_HASH(unsigned char)
ETL_DEFINE_TRIVIAL_HASH(wchar_t)
ETL_DEFINE_TRIVIAL_HASH(char16_t)
ETL_DEFINE_TRIVIAL_HASH(char32_t)
ETL_DEFINE_TRIVIAL_HASH(short)
ETL_DEFINE_TRIVIAL_HASH(unsigned short)
ETL_DEFINE_TRIVIAL_HASH(int)
ETL_DEFINE_TRIVIAL_HASH(unsigned int)
ETL_DEFINE_TRIVIAL_HASH(long)
ETL_DEFINE_TRIVIAL_HASH(unsigned long)
ETL_DEFINE_TRIVIAL_HASH(long long)
ETL_DEFINE_TRIVIAL_HASH(unsigned long long)

}

#else   // ETL_HAS_CPP11 == 1

namespace ETL_NAMESPACE {
using std::hash;
}

#endif


namespace ETL_NAMESPACE {

template<class T>
inline void hash_combine(std::size_t& seed, const T& v) {

    std::hash<T> hasher;
    seed = Detail::hashCombineMethod(seed, hasher(v));
}

}

#endif /* __ETL_HASH_H__ */
