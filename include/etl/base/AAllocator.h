/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2018 Balazs Toth.

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

namespace ETL_NAMESPACE {

/**
Allocator interface class.
\tparam T Item type
*/
template<class T>
class AAllocator {

  public:   // types

    typedef T ItemType;
    typedef T* PtrType;

  public:   // functions

    virtual ~AAllocator() {};

    virtual size_t max_size() const = 0;
    virtual size_t size() const = 0;

    virtual PtrType allocate(uint32_t n) = 0;
    virtual void deallocate(PtrType ptr, uint32_t n) = 0;

    virtual const void* handle() const = 0;

    static void construct(PtrType ptr) {
        new (ptr) ItemType;
    }

    static void construct(PtrType ptr, const ItemType& other) {
        new (ptr) ItemType(other);
    }

    template<typename... Args >
    static void construct(PtrType ptr, Args&& ... args) {
        new (ptr) ItemType(std::forward<Args>(args)...);
    }

    static void destroy(PtrType ptr) {
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

  public:   // types

    typedef T ItemType;
    typedef T* PtrType;

    typedef A<T> Allocator;

  public:   // functions

    size_t max_size() const override {
        return allocator().max_size();
    }

    size_t size() const override {
        return allocator().max_size();
    }

    PtrType allocate(uint32_t n) override {
        return allocator().allocate(n);
    }

    void deallocate(PtrType ptr, uint32_t n) override {
        allocator().deallocate(ptr, n);
    }

    const void* handle() const override {
        return &allocator();
    }

  private:

    static Allocator& allocator() {
        static Allocator alloc;
        return alloc;
    }

};

}

#endif /* __ETL_AALLOCATOR_H__ */

