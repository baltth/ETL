/**
\file
\date 2016.01.20. 19:44:12
\author Tóth Balázs - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Tóth Balázs.

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

#ifndef __ETL_VECTORTEMPLATE_03_H__
#define __ETL_VECTORTEMPLATE_03_H__

#undef min
#undef max

#include "langSupport.h"

#if (ETL_USE_CPP11 == 0)

#include <new>
#include <utility>

#include "Base/TypedVectorBase.h"
#include "Base/MemStrategies.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {


template<class T>
class VectorTemplate : public TypedVectorBase<T>, public DynamicResizeStrategy<VectorTemplate<T> > {
friend DynamicResizeStrategy<VectorTemplate<T> >;  

// types
public:

    typedef T ItemType;
    typedef T* Iterator;

    typedef typename TypedVectorBase<T>::CreatorFunctor CreatorFunctor;

// functions
public:

    inline Iterator insert(Iterator position, const T &value) {
        return insert(position, 1, value);
    }

    Iterator insert(Iterator position, uint32_t num, const T &value);

    inline void pushFront(const T &value) {
        insert(TypedVectorBase<T>::begin(), value);
    }

    inline void pushBack(const T &value) {
        insert(TypedVectorBase<T>::end(), value);
    }

    inline void swap(VectorTemplate<T> &other) {
        AVectorBase::swap(other);
    }

protected:

    VectorTemplate<T>() {};

    explicit VectorTemplate<T>(uint32_t len);

    VectorTemplate<T>(uint32_t len, const T &item);

    VectorTemplate<T>(const VectorTemplate<T> &other);
    VectorTemplate<T> &operator=(const VectorTemplate<T> &other);

    ~VectorTemplate<T>() {
        TypedVectorBase<T>::clear();
    }

private:


    Iterator insertWithCreator(Iterator position, uint32_t num, const CreatorFunctor& creatorCall);

};


template<class T>
VectorTemplate<T>::VectorTemplate(uint32_t len) {

    typename TypedVectorBase<T>::DefaultCreator dc;
    insertWithCreator(TypedVectorBase<T>::begin(), len, dc);
}


template<class T>
VectorTemplate<T>::VectorTemplate(uint32_t len, const T &item) {

    insert(TypedVectorBase<T>::begin(), len, item);
}


template<class T>
VectorTemplate<T>::VectorTemplate(const VectorTemplate<T> &other) {

    operator=(other);
}


template<class T>
VectorTemplate<T> &VectorTemplate<T>::operator=(const VectorTemplate<T> &other) {

    reserve(other.getSize());
    copyOperation(other.begin(), other.getSize());
    return *this;
}


template<class T>
typename VectorTemplate<T>::Iterator VectorTemplate<T>::insert(Iterator position, uint32_t num, const T &value) {

    typename TypedVectorBase<T>::CopyCreator cc(value);
    return insertWithCreator(position, num, cc);
}


template<class T>
typename VectorTemplate<T>::Iterator VectorTemplate<T>::insertWithCreator(Iterator position,
                                                                          uint32_t numToInsert,
                                                                          const CreatorFunctor& creatorCall) {

    if(numToInsert > 0) {

        if((TypedVectorBase<T>::getSize() + numToInsert) > TypedVectorBase<T>::getCapacity()) {

            uint32_t positionIx = position - TypedVectorBase<T>::begin();
            this->reserveAtLeast(this->getSize() + numToInsert);
            position = TypedVectorBase<T>::begin() + positionIx;
        }

        position = TypedVectorBase<T>::insertOperation(position, numToInsert, creatorCall);
    }

    return position;
}

}

#endif

#endif /* __ETL_VECTORTEMPLATE_03_H__ */

