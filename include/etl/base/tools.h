/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2022 Balazs Toth.

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

#ifndef __ETL_TOOLS_H__
#define __ETL_TOOLS_H__

#include <etl/etlSupport.h>
#include <etl/traitSupport.h>

namespace ETL_NAMESPACE {
namespace Detail {


template<class T, class S = void>
struct TypeDefined {
    typedef S type;
};


template<class T, class Enable = void>
struct HasFirstType : std::false_type {};

template<class T>
struct HasFirstType<T, typename TypeDefined<typename T::first_type>::type> : std::true_type {};


template<class T, class Enable = void>
struct HasValueType : std::false_type {};

template<class T>
struct HasValueType<T, typename TypeDefined<typename T::value_type>::type> : std::true_type {};


template<class L,
         class R,
         typename = enable_if_t<HasValueType<L>::value && HasValueType<R>::value>,
         typename = enable_if_t<is_same<typename L::value_type, typename R::value_type>::value>>
bool isEqual(const L& lhs, const R& rhs) {

    if (&lhs == &rhs) {
        return true;
    } else if (lhs.size() != rhs.size()) {
        return false;
    } else {

        bool eq = true;
        auto lIt = lhs.begin();
        auto rIt = rhs.begin();
        while (eq && (lIt != lhs.end())) {
            eq = (*lIt == *rIt);
            ++lIt;
            ++rIt;
        }

        return eq;
    }
}


template<class L,
         class R,
         typename = enable_if_t<HasValueType<L>::value && HasValueType<R>::value>,
         typename = enable_if_t<is_same<typename L::value_type, typename R::value_type>::value>>
bool isLess(const L& lhs, const R& rhs) {

    if (&lhs == &rhs) {
        return false;
    } else if (lhs.size() < rhs.size()) {
        return true;
    } else if (lhs.size() > rhs.size()) {
        return false;
    } else {

        bool less = false;
        bool greater = false;
        auto lIt = lhs.begin();
        auto rIt = rhs.begin();
        while ((!less) && (!greater) && (lIt != lhs.end())) {
            less = (*lIt < *rIt);
            greater = (*lIt > *rIt);
            ++lIt;
            ++rIt;
        }

        return less;
    }
}


struct SizeDiff {
    uint32_t common {0};
    uint32_t lGreaterWith {0};
    uint32_t rGreaterWith {0};
};


template<class L, class R>
enable_if_t<is_integral<L>::value && is_integral<R>::value, SizeDiff> sizeDiff(L l, R r) {

    SizeDiff d {};

    if (l > r) {
        d.common = r;
        d.lGreaterWith = l - r;
    } else {
        d.common = l;
        d.rGreaterWith = r - l;
    }

    return d;
}

template<class L, class R>
enable_if_t<!(is_integral<L>::value && is_integral<R>::value), SizeDiff> sizeDiff(const L& l,
                                                                                  const R& r) {
    return sizeDiff(l.size(), r.size());
}


template<class T>
struct NothrowContract {

    static constexpr bool nothrowIfDefaultConstructible =
        std::is_nothrow_default_constructible<T>::value
        || (!std::is_default_constructible<T>::value);

    static constexpr bool nothrowIfMoveConstructible =
        std::is_nothrow_move_constructible<T>::value || (!std::is_move_constructible<T>::value);

    static constexpr bool nothrowIfMoveAssignable =
        std::is_nothrow_move_assignable<T>::value || (!std::is_move_assignable<T>::value);

    static constexpr bool nothrowIfDestructible =
        std::is_nothrow_destructible<T>::value || (!std::is_destructible<T>::value);

    static constexpr bool nothrowIfMovable = nothrowIfMoveConstructible && nothrowIfMoveAssignable;
    static constexpr bool value =
        nothrowIfDefaultConstructible && nothrowIfMovable && nothrowIfDestructible;
};


template<typename S, typename T>
struct CopyConst {
    typedef T Type;
};

template<typename S, typename T>
struct CopyConst<const S, T> {
    typedef const T Type;
};


template<typename T>
constexpr typename std::add_const<T>::type& asConst(T& t) noexcept {
    return t;
}

template<typename T>
constexpr typename std::add_const<T>::type* asConst(T* t) noexcept {
    return t;
}

template<typename T>
void asConst(const T&&) = delete;


/// Trait struct for customizing e.g. `List::swapTwo()` operations.
/// The default implementation checks the standard contract of `swap()` function.
/// Rationale: stdlib implementations may have inconsistent traits,
/// e.g. with gcc 5 `std::is_move_assignable<std::pair<const K, E>>::value == true`
/// This induces using `swap()` operation in certain situations, but that
/// does not compile. This struct allows to force 'stealing' instead of
/// swapping via specialization.
template<typename T>
struct UseSwapInCont {
    static constexpr bool value = is_move_constructible<T>::value && is_move_assignable<T>::value;
};

template<class K, class E>
struct UseSwapInCont<std::pair<const K, E>> {
    static constexpr bool value = false;
};


}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // __ETL_TOOLS_H__
