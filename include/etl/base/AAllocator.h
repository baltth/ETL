/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2018-2024 Balazs Toth.

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

#ifndef ETL_AALLOCATOR_H_
#define ETL_AALLOCATOR_H_

#include <etl/etlSupport.h>

#include <new>
#include <type_traits>

namespace ETL_NAMESPACE {

/**
Allocator interface class.
\tparam T Item type
*/
template<class T>
class AAllocator {

  public:  // types

    using ItemType = T;
    using PtrType = T*;

    static constexpr bool noexceptDestroy = std::is_nothrow_destructible<T>::value;

  public:  // functions

    virtual ~AAllocator() = default;

    virtual std::size_t max_size() const noexcept = 0;
    virtual std::size_t size() const noexcept = 0;
    virtual std::size_t reserve() const noexcept = 0;

    virtual PtrType allocate(std::size_t n) = 0;
    virtual void deallocate(PtrType ptr, std::size_t n) noexcept = 0;

    virtual const void* handle() const noexcept = 0;

    static void construct(PtrType ptr) noexcept(noexcept(new (ptr) ItemType)) {
        new (ptr) ItemType;
    }

    template<typename... Args>
    static void
    construct(PtrType ptr,
              Args&&... args) noexcept(noexcept(new (ptr) ItemType(std::forward<Args>(args)...))) {
        new (ptr) ItemType(std::forward<Args>(args)...);
    }

    static void destroy(PtrType ptr) noexcept(noexceptDestroy) {
        ptr->~ItemType();
    }
};


/**
Allocator template class for forwarding std concept allocators.
\tparam T Item type
\tparam A allocator type to forward, defaults to std::allocator
*/
template<class T, template<class> class A = std::allocator>
class AllocatorWrapper : public AAllocator<T> {

  public:  // types

    using ItemType = T;
    using PtrType = T*;

    using Allocator = A<T>;

    static constexpr bool uniqueAllocator = false;

  public:  // functions

    std::size_t max_size() const noexcept override {
        return allocator().max_size();
    }

    std::size_t size() const noexcept override {
        return 0U;
    }

    std::size_t reserve() const noexcept override {
        return max_size();
    }

    PtrType allocate(std::size_t n) override {
        return allocator().allocate(n);
    }

    void deallocate(PtrType ptr, std::size_t n) noexcept override {
        allocator().deallocate(ptr, n);
    }

    const void* handle() const noexcept override {
        return &allocator();
    }

  private:

    static Allocator& allocator() {
        static Allocator alloc;
        return alloc;
    }
};


namespace Detail {

template<class T, template<class> class A>
struct AllocatorTraits {

    template<class S, typename = void>
    struct hasUniqueAllocatorTag : std::false_type {};

    template<class S>
    struct hasUniqueAllocatorTag<S, decltype(S::uniqueAllocator)> : std::true_type {};

    template<class S>
    static constexpr bool uniqueAllocatorTag(std::false_type) {
        return false;
    }

    template<class S>
    static constexpr bool uniqueAllocatorTag(std::true_type) {
        return S::uniqueAllocator;
    }

    static constexpr bool isChildOfAAllocator = std::is_base_of<AAllocator<T>, A<T>>::value;
    static constexpr bool uniqueAllocator =
        isChildOfAAllocator ? uniqueAllocatorTag<A<T>>(hasUniqueAllocatorTag<A<T>> {}) : false;

    using Type = typename std::conditional<isChildOfAAllocator, A<T>, AllocatorWrapper<T, A>>::type;
};

}  // namespace Detail

}  // namespace ETL_NAMESPACE

#endif  // ETL_AALLOCATOR_H_
