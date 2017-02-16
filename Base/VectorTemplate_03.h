﻿/**
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

#include "Base/AVectorBase.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {


template<class T>
class VectorTemplate : public AVectorBase {

// types
public:

    typedef T* Iterator;

private:

    class CreatorFunctor {
        public:
            virtual void call(T* pos, bool place) const = 0;
    };

    class DefaultCreator : public CreatorFunctor {
        public:
            virtual void call(T* pos, bool place) const {
                if(place) {
                    placeDefaultTo(pos);
                } else {
                    assignDefaultTo(pos);
                }
            }
    };

    class CopyCreator : public CreatorFunctor {
        private:
            const T& ref;    

        public:
            CopyCreator(const T& refValue) :
                ref(refValue) {};

            virtual void call(T* pos, bool place) const {
                if(place) {
                    placeValueTo(pos, ref);
                } else {
                    assignValueTo(pos, ref);
                }
            }
    };

// functions
public:

    inline T &operator[](int32_t ix) {
        return *(static_cast<T*>(getItemPointer(ix)));
    }

    inline const T &operator[](int32_t ix) const {
        return *(static_cast<T*>(getItemPointer(ix)));
    }

    inline Iterator begin() const {
        return static_cast<Iterator>(getItemPointer(0));
    }

    inline Iterator end() const {
        return static_cast<Iterator>(getItemPointer(getSize()));
    }

    inline T &front() {
        return *(static_cast<T*>(getItemPointer(0)));
    }

    inline const T &front() const {
        return *(static_cast<T*>(getItemPointer(0)));
    }

    inline T &back() {
        return *(static_cast<T*>(getItemPointer(getSize() - 1)));
    }

    inline const T &back() const {
        return *(static_cast<T*>(getItemPointer(getSize() - 1)));
    }

    inline T* getData() {
        return static_cast<T*>(getItemPointer(0));
    }

    inline const T* getData() const {
        return static_cast<T*>(getItemPointer(0));
    }

    void reserve(uint32_t length) OVERRIDE;
    void shrinkToFit() OVERRIDE;
    void resize(uint32_t newLength) OVERRIDE;
    void clear() OVERRIDE;

    inline Iterator insert(Iterator position, const T &value) {
        return insert(position, 1, value);
    }

    Iterator insert(Iterator position, uint32_t num, const T &value);

    inline Iterator erase(Iterator pos) {
        Iterator next = pos;
        ++next;
        return erase(pos, next);
    }

    Iterator erase(Iterator first, Iterator last);

    inline void pushFront(const T &value) {
        insert(begin(), value);
    }

    inline void popFront() {
        erase(begin());
    }

    inline void pushBack(const T &value) {
        insert(end(), value);
    }

    inline void popBack() {
        erase(end());
    }

    inline void swap(VectorTemplate<T> &other) {
        AVectorBase::swap(other);
    }

protected:

    VectorTemplate<T>() :
        AVectorBase(sizeof(T)) {}

    explicit VectorTemplate<T>(uint32_t len);

    VectorTemplate<T>(uint32_t len, const T &item);

    VectorTemplate<T>(const VectorTemplate<T> &other);
    VectorTemplate<T> &operator=(const VectorTemplate<T> &other);

    ~VectorTemplate<T>() {
        clear();
    }

    static inline uint32_t getRoundedLength(uint32_t length) {
        return (length + (RESIZE_STEP - 1)) & ~(RESIZE_STEP - 1);
    }

    static inline void assignDefaultTo(T* ptr) {
        *ptr = T();
    }

    static inline void placeDefaultTo(T* ptr) {
        new(ptr) T();
    }

    static void assignValueTo(T* ptr, const T &value) {
        *ptr = value;
    }

    static void placeValueTo(T* ptr, const T &value) {
        new(ptr) T(value);
    }

private:

    void initWith(const T* src, uint32_t num);
    void copyOperation(const T* src, uint32_t num);
    void reallocateAndCopyFor(uint32_t len);
    void decreaseSizeTo(uint32_t len);

    void uninitializedCopy(T* src, T* dst, uint32_t num);
    void initializedCopyUp(T* src, T* dst, uint32_t num);
    void initializedCopyDown(T* src, T* dst, uint32_t num);

    void destruct(Iterator startPos, Iterator endPos);

    Iterator insertWithCreator(Iterator position, uint32_t num, const CreatorFunctor& creatorCall);

};


template<class T>
VectorTemplate<T>::VectorTemplate(uint32_t len) :
    AVectorBase(sizeof(T)) {

    DefaultCreator dc;
    insertWithCreator(begin(), len, dc);
}


template<class T>
VectorTemplate<T>::VectorTemplate(uint32_t len, const T &item) :
    AVectorBase(sizeof(T)) {

    insert(begin(), len, item);
}


template<class T>
VectorTemplate<T>::VectorTemplate(const VectorTemplate<T> &other) :
    AVectorBase(sizeof(T)) {

    initWith(other.begin(), other.getSize());
}


template<class T>
VectorTemplate<T> &VectorTemplate<T>::operator=(const VectorTemplate<T> &other) {

    copyOperation(other.begin(), other.getSize());
    return *this;
}


template<class T>
void VectorTemplate<T>::initWith(const T* src, uint32_t num) {

    reserve(num);

    T* dataAlias = getData();

    for(uint32_t i = 0; i < num; ++i) {
        placeValueTo((dataAlias + i), src[i]);
    }

    numElements = num;
}


template<class T>
void VectorTemplate<T>::copyOperation(const T* src, uint32_t num) {

    reserve(num);

    T* dataAlias = getData();
    uint32_t i = 0;

    for(; i < getSize(); ++i) {
        assignValueTo((dataAlias + i), src[i]);
    }

    for(; i < num; ++i) {
        placeValueTo((dataAlias + i), src[i]);
    }

    destruct((dataAlias + i), end());
    numElements = num;
}


template<class T>
void VectorTemplate<T>::clear() {

    destruct(begin(), end());
    numElements = 0;
}


template<class T>
void VectorTemplate<T>::reserve(uint32_t length) {

    if(length > getCapacity()) {
        reallocateAndCopyFor(length);
    }
}


template<class T>
void VectorTemplate<T>::shrinkToFit() {

    if(getCapacity() > getSize()) {
        reallocateAndCopyFor(getSize());
    }
}


template<class T>
void VectorTemplate<T>::resize(uint32_t newLength) {

    if(newLength > getSize()) {

        if(newLength > getCapacity()) {
            reallocateAndCopyFor(getRoundedLength(newLength));
        }

        Iterator newEnd = getData() + newLength;

        for(Iterator it = end(); it < newEnd; ++it) {
            placeDefaultTo(it);
        }

    } else if(newLength < getSize()) {

        Iterator newEnd = getData() + newLength;
        destruct(newEnd, end());
    }

    numElements = newLength;
}


template<class T>
void VectorTemplate<T>::reallocateAndCopyFor(uint32_t len) {

    T* oldData = getData();
    allocate(len);

    if(oldData != NULLPTR) {

        uint32_t numToCopy = (len < numElements) ? len : numElements;

        if((getData() != NULLPTR) && (numToCopy > 0)) {

            T* dataAlias = getData();
            uninitializedCopy(oldData, dataAlias, numToCopy);
        }

        AVectorBase::deallocatePtr(oldData);
    }
}


template<class T>
typename VectorTemplate<T>::Iterator VectorTemplate<T>::insert(Iterator position, uint32_t num, const T &value) {

    CopyCreator cc(value);
    return insertWithCreator(position, num, cc);
}


template<class T>
typename VectorTemplate<T>::Iterator VectorTemplate<T>::insertWithCreator(Iterator position,
                                                                          uint32_t numToInsert,
                                                                          const CreatorFunctor& creatorCall) {

    if(numToInsert > 0) {

        if((getSize() + numToInsert) > getCapacity()) {

            uint32_t positionIx = position - begin();
            reallocateAndCopyFor(getRoundedLength(getSize() + numToInsert));
            position = begin() + positionIx;
        }

        uint32_t distanceFromEnd = end() - position;

        uint32_t uninitedCopyNumber = (distanceFromEnd >= numToInsert) ? numToInsert : distanceFromEnd;
        uint32_t initedCopyNumber = (distanceFromEnd >= numToInsert) ? (distanceFromEnd - numToInsert) : 0;
        uint32_t uninitedInsertNumber = (distanceFromEnd >= numToInsert) ? 0 : (numToInsert - distanceFromEnd);
        uint32_t initedInsertNumber = uninitedCopyNumber;

        T* src = end() - uninitedCopyNumber;
        T* dst = end() + numToInsert - uninitedCopyNumber;

        uninitializedCopy(src, dst, uninitedCopyNumber);

        src -= initedCopyNumber;
        dst -= initedCopyNumber;

        initializedCopyUp(src, dst, initedCopyNumber);

        T* uninitedInsertPos = dst;
        dst -= uninitedInsertNumber;

        while(uninitedInsertPos > dst) {
            --uninitedInsertPos;
            creatorCall.call(uninitedInsertPos, true);
        }

        dst -= initedInsertNumber;

        while(uninitedInsertPos > dst) {
            --uninitedInsertPos;
            creatorCall.call(uninitedInsertPos, false);
        }

        numElements += numToInsert;
    }

    return position;
}


template<class T>
typename VectorTemplate<T>::Iterator VectorTemplate<T>::erase(Iterator first, Iterator last) {

    int numToErase = last - first;
    Iterator itAfterDeleted = first;

    if(numToErase > 0) {

        uint32_t numToMove = end() - last;
        initializedCopyDown(&*last, &*first, numToMove);

        first += numToMove;
        destruct(first, end());

        numElements -= numToErase;
    }

    return itAfterDeleted;
}


template<class T>
void VectorTemplate<T>::destruct(Iterator startPos, Iterator endPos) {

    while(startPos < endPos) {          // operator<() instead of !=() : protection for startPos > endPos cases
        startPos->~T();
        ++startPos;
    }
}


template<class T>
void VectorTemplate<T>::uninitializedCopy(T* src, T* dst, uint32_t num) {

    if(src != dst) {
        for(int i = (num - 1); i >= 0; --i) {               // uninitializedCopy() always copies upwards
            placeValueTo((dst + i), src[i]);                // Placement new, copy constuctor
        }
    }
}


template<class T>
void VectorTemplate<T>::initializedCopyUp(T* src, T* dst, uint32_t num) {

    if(src != dst) {
        for(int i = (num - 1); i >= 0; --i) {
            assignValueTo((dst + i), src[i]);
        }
    }
}


template<class T>
void VectorTemplate<T>::initializedCopyDown(T* src, T* dst, uint32_t num) {

    if(src != dst) {
        for(uint32_t i = 0; i < num; ++i) {
            assignValueTo((dst + i), src[i]);
        }
    }
}

}

#endif

#endif /* __ETL_VECTORTEMPLATE_03_H__ */

