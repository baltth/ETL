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

#ifndef __ETL_FUNCSUPPORT_H__
#define __ETL_FUNCSUPPORT_H__

#include <etl/etlSupport.h>


#if (ETL_HAS_CPP11 == 0)


// Type trait implementations mainly from https://en.cppreference.com

namespace ETL_NAMESPACE {

namespace Detail {

template<class T>
struct addrImplRef {

    T& v_;

    inline addrImplRef(T& v): v_(v) {}
    inline operator T& () const {
        return v_;
    }

  private:

    addrImplRef& operator=(const addrImplRef&);

};

template<class T>
struct addressofImpl {

    static inline T* f(T& v, long) {
        return reinterpret_cast<T*>(
                   &const_cast<char&>(reinterpret_cast<const volatile char&>(v)));
    }

    static inline T* f(T* v, int) {
        return v;
    }
};

}

template<class T>
T* addressof(T& v) {
    return Detail::addressofImpl<T>::f(Detail::addrImplRef<T>(v), 0);
}


// Note: reference_wrapper currently does not implement operator()
template <class T>
class reference_wrapper {

  public:   // types

    typedef T type;

  private:  // variables

    T* ptr;

  public:   // functions

    reference_wrapper(T& ref) :
        ptr(ETL_NAMESPACE::addressof(ref)) {};

    operator T& () const {
        return *ptr;
    }

    T& get() const {
        return *ptr;
    }

  private:

    reference_wrapper();

};


template<class T>
reference_wrapper<T> ref(T& t) {
    return reference_wrapper<T>(t);
}

template<class T>
reference_wrapper<T> ref(reference_wrapper<T> t) {
    return ref(t.get());
}

template<class T>
reference_wrapper<const T> cref(const T& t) {
    return reference_wrapper<T>(t);
}

template<class T>
reference_wrapper<const T> cref(reference_wrapper<const T> t) {
    return cref(t.get());
}

}

#else /* ETL_USE_CPP11 == 1 */

#include <memory>
#include <functional>

namespace ETL_NAMESPACE {

using std::addressof;

using std::reference_wrapper;
using std::ref;
using std::cref;

}

#endif


#endif /* __ETL_FUNCSUPPORT_H__ */

