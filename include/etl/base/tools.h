/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2024 Balazs Toth.

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

#ifndef ETL_TOOLS_H_
#define ETL_TOOLS_H_

#include <etl/etlSupport.h>
#include <etl/traitSupport.h>

#include <iterator>

namespace ETL_NAMESPACE {
namespace Detail {


template<class T, class S = void>
struct TypeDefined {
    using type = S;
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


template<typename T, typename = void>
struct IsInputIterator {
    static constexpr bool value = false;
};


template<typename T>
struct IsInputIterator<
    T,
    void_t<typename std::iterator_traits<remove_reference_t<T>>::iterator_category>> {
    static constexpr bool value =
        is_convertible<typename std::iterator_traits<remove_reference_t<T>>::iterator_category,
                       std::input_iterator_tag>::value;
};

static_assert(!IsInputIterator<int>::value, "IsInputIterator<> error");
static_assert(IsInputIterator<int*>::value, "IsInputIterator<> error");


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


template<class T>
const bool NothrowContract<T>::nothrowIfDefaultConstructible;
template<class T>
const bool NothrowContract<T>::nothrowIfMoveConstructible;
template<class T>
const bool NothrowContract<T>::nothrowIfMoveAssignable;
template<class T>
const bool NothrowContract<T>::nothrowIfDestructible;
template<class T>
const bool NothrowContract<T>::nothrowIfMovable;
template<class T>
const bool NothrowContract<T>::value;


template<typename S, typename T>
struct CopyConst {
    using Type = T;
};

template<typename S, typename T>
struct CopyConst<const S, T> {
    using Type = const T;
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


class NullLock {
  public:  // functions
    void lock() noexcept {}
    void unlock() noexcept {}
};


template<class L>
class LockGuard {

  private:  // variables

    L* l;

  public:  // functions

    explicit LockGuard(L& toLock) noexcept(noexcept(l->lock())) :
        l {&toLock} {
        l->lock();
    }

    LockGuard() = delete;
    LockGuard(const LockGuard& other) = delete;
    LockGuard& operator=(const LockGuard& other) & = delete;
    LockGuard& operator=(LockGuard&& other) = delete;

    LockGuard(LockGuard&& other) noexcept :
        l {other.l} {
        other.l = nullptr;
    }

    ~LockGuard() {
        if (l != nullptr) {
            l->unlock();
        }
    }
};


template<class L>
LockGuard<L> lock(L& toLock) noexcept(noexcept(LockGuard<L> {toLock})) {
    return LockGuard<L> {toLock};
}

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // ETL_TOOLS_H_
