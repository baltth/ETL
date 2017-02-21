/**
\file
\date 2017.02.16. 14:14:13
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

#ifndef __ETL_TYPEDVECTORBASE_H__
#define __ETL_TYPEDVECTORBASE_H__

#undef min
#undef max

#include "langSupport.h"

#if ETL_USE_CPP11
#include <functional>
#include <initializer_list>
#endif

#include <new>
#include <utility>

#include "Base/AVectorBase.h"

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {


template<class T>
class TypedVectorBase : public AVectorBase {

// types
public:

    typedef T ItemType;
    typedef T* Iterator;

protected:

#if ETL_USE_CPP11

    typedef std::function<void(T*, bool)> CreatorFunctor;

#else 

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

#endif

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

    inline void popFront() {
        erase(begin());
    }

    inline void popBack() {
        erase(end() - 1);
    }
    
    inline Iterator erase(Iterator pos) {
        Iterator next = pos;
        ++next;
        return erase(pos, next);
    }

    Iterator erase(Iterator first, Iterator last);

    void clear();

protected:

    TypedVectorBase() :
        AVectorBase(sizeof(T)) {};

    ~TypedVectorBase() {
        clear();
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

#if ETL_USE_CPP11

    static void assignValueTo(T* ptr, T &&value) {
        *ptr = std::move(value);
    }

    static void placeValueTo(T* ptr, T &&value) {
        new(ptr) T(std::move(value));
    }

#endif

    void copyOperation(const T* src, uint32_t num);
    
    void uninitializedCopy(T* src, T* dst, uint32_t num);
    void initializedCopyUp(T* src, T* dst, uint32_t num);
    void initializedCopyDown(T* src, T* dst, uint32_t num);

    void destruct(Iterator startPos, Iterator endPos);

#if ETL_USE_CPP11

    Iterator insertOperation(Iterator position, uint32_t num, CreatorFunctor&& creatorCall);

#else

    Iterator insertOperation(Iterator position, uint32_t num, const CreatorFunctor& creatorCall);

#endif

};


template<class T>
typename TypedVectorBase<T>::Iterator TypedVectorBase<T>::erase(Iterator first, Iterator last) {

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
void TypedVectorBase<T>::copyOperation(const T* src, uint32_t num) {

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


#if ETL_USE_CPP11

template<class T>
typename TypedVectorBase<T>::Iterator TypedVectorBase<T>::insertOperation(Iterator position,
                                                                          uint32_t numToInsert,
                                                                          CreatorFunctor&& creatorCall) {

    if(numToInsert > 0) {

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
            creatorCall(uninitedInsertPos, true);
        }

        dst -= initedInsertNumber;

        while(uninitedInsertPos > dst) {
            --uninitedInsertPos;
            creatorCall(uninitedInsertPos, false);
        }

        numElements += numToInsert;
    }

    return position;
}

#else

template<class T>
typename TypedVectorBase<T>::Iterator TypedVectorBase<T>::insertOperation(Iterator position,
                                                                          uint32_t numToInsert,
                                                                          const CreatorFunctor& creatorCall) {

    if(numToInsert > 0) {

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

#endif


template<class T>
void TypedVectorBase<T>::uninitializedCopy(T* src, T* dst, uint32_t num) {

    if(src != dst) {
        for(int i = (num - 1); i >= 0; --i) {               // uninitializedCopy() always copies upwards
#if ETL_USE_CPP11
            placeValueTo((dst + i), std::move(src[i]));     // Placement new, move constuctor
#else
            placeValueTo((dst + i), src[i]);                // Placement new, copy constuctor
#endif
        }
    }
}


template<class T>
void TypedVectorBase<T>::initializedCopyUp(T* src, T* dst, uint32_t num) {

    if(src != dst) {
        for(int i = (num - 1); i >= 0; --i) {
#if ETL_USE_CPP11
            assignValueTo((dst + i), std::move(src[i]));
#else
            assignValueTo((dst + i), src[i]);
#endif
        }
    }
}


template<class T>
void TypedVectorBase<T>::initializedCopyDown(T* src, T* dst, uint32_t num) {

    if(src != dst) {
        for(uint32_t i = 0; i < num; ++i) {
#if ETL_USE_CPP11
            assignValueTo((dst + i), std::move(src[i]));
#else
            assignValueTo((dst + i), src[i]);
#endif
        }
    }
}


template<class T>
void TypedVectorBase<T>::clear() {

    destruct(begin(), end());
    numElements = 0;
}


template<class T>
void TypedVectorBase<T>::destruct(Iterator startPos, Iterator endPos) {

    while(startPos < endPos) {          // operator<() instead of !=() : protection for startPos > endPos cases
        startPos->~T();
        ++startPos;
    }
}

}

#endif /* __ETL_TYPEDVECTORBASE_H__ */

