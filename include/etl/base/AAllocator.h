/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2018-2022 Balazs Toth.

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

#ifndef __ETL_AALLOCATOR_H__
#define __ETL_AALLOCATOR_H__

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

    typedef T ItemType;
    typedef T* PtrType;

    static constexpr bool NoexceptDestroy = std::is_nothrow_destructible<T>::value;

  public:  // functions

    virtual ~AAllocator() {};

    virtual size_t max_size() const noexcept = 0;
    virtual size_t size() const noexcept = 0;

    virtual PtrType allocate(uint32_t n) = 0;
    virtual void deallocate(PtrType ptr, uint32_t n) noexcept = 0;

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

    static void destroy(PtrType ptr) noexcept(NoexceptDestroy) {
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

    static constexpr bool UNIQUE_ALLOCATOR = false;

  public:  // functions

    size_t max_size() const noexcept override {
        return allocator().max_size();
    }

    size_t size() const noexcept override {
        return allocator().max_size();
    }

    PtrType allocate(uint32_t n) override {
        return allocator().allocate(n);
    }

    void deallocate(PtrType ptr, uint32_t n) noexcept override {
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


template<class T, template<class> class A>
struct AllocatorTraits {

    static constexpr bool IS_CHILD_OF_AALLOCATOR = std::is_base_of<AAllocator<T>, A<T>>::value;
    static constexpr bool UNIQUE_ALLOCATOR =
        IS_CHILD_OF_AALLOCATOR ? A<T>::UNIQUE_ALLOCATOR : false;
    using Type =
        typename std::conditional<IS_CHILD_OF_AALLOCATOR, A<T>, AllocatorWrapper<T, A>>::type;
};


}  // namespace ETL_NAMESPACE

#endif  // __ETL_AALLOCATOR_H__
