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

#include "etlSupport.h"

#include "Base/AVectorBase.h"

#if ETL_USE_EXCEPTIONS
#include "ExceptionTypes.h"
#endif

#undef min
#undef max

#include <new>
#include <utility>

#if ETL_USE_CPP11
#include <functional>
#include <initializer_list>
#endif

namespace ETL_NAMESPACE {

template<class> class StaticSized;
template<class, class> class DynamicSized;


template<class T>
class TypedVectorBase : public AVectorBase {
    friend StaticSized<TypedVectorBase>;
    template<class C, class A>
    friend class DynamicSized;

  public:   // types

    typedef T ItemType;
    typedef T* Iterator;
    typedef const T* ConstIterator;

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
            if (place) {
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
        explicit CopyCreator(const T& refValue) :
            ref(refValue) {};

        virtual void call(T* pos, bool place) const {
            if (place) {
                placeValueTo(pos, ref);
            } else {
                assignValueTo(pos, ref);
            }
        }
    };

#endif

  public:   // functions

    inline T& operator[](uint32_t ix) {
        return *(static_cast<T*>(getItemPointer(ix)));
    }

    inline const T& operator[](uint32_t ix) const {
        return *(static_cast<T*>(getItemPointer(ix)));
    }

#if ETL_USE_EXCEPTIONS

    T& at(uint32_t ix);
    const T& at(uint32_t ix) const;

#endif

    inline Iterator begin() {
        return static_cast<Iterator>(getItemPointer(0));
    }

    inline ConstIterator begin() const {
        return static_cast<ConstIterator>(getItemPointer(0));
    }

    inline Iterator end() {
        return static_cast<Iterator>(getItemPointer(getSize()));
    }

    inline ConstIterator end() const {
        return static_cast<ConstIterator>(getItemPointer(getSize()));
    }

    inline T& front() {
        return *(static_cast<T*>(getItemPointer(0)));
    }

    inline const T& front() const {
        return *(static_cast<T*>(getItemPointer(0)));
    }

    inline T& back() {
        return *(static_cast<T*>(getItemPointer(getSize() - 1)));
    }

    inline const T& back() const {
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
        new (ptr) T();
    }

    static void assignValueTo(T* ptr, const T& value) {
        *ptr = value;
    }

    static void placeValueTo(T* ptr, const T& value) {
        new (ptr) T(value);
    }

#if ETL_USE_CPP11

    static void assignValueTo(T* ptr, T&& value) {
        *ptr = std::move(value);
    }

    static void placeValueTo(T* ptr, T&& value) {
        new (ptr) T(std::move(value));
    }

#endif

    void copyOperation(const T* src, uint32_t num);

    static void uninitializedCopy(T* src, T* dst, uint32_t num);
    static void initializedCopyUp(T* src, T* dst, uint32_t num);
    static void initializedCopyDown(T* src, T* dst, uint32_t num);

    static void destruct(Iterator startPos, Iterator endPos);

#if ETL_USE_CPP11

    Iterator insertOperation(ConstIterator position, uint32_t num, CreatorFunctor&& creatorCall);

#else

    Iterator insertOperation(ConstIterator position, uint32_t num, const CreatorFunctor& creatorCall);

#endif

};


template<class T>
typename TypedVectorBase<T>::Iterator TypedVectorBase<T>::erase(Iterator first, Iterator last) {

    int numToErase = last - first;
    Iterator itAfterDeleted = first;

    if (numToErase > 0) {

        uint32_t numToMove = end() - last;
        initializedCopyDown(&*last, &*first, numToMove);

        first += numToMove;
        destruct(first, end());

        proxy.setSize(getSize() - numToErase);
    }

    return itAfterDeleted;
}


template<class T>
void TypedVectorBase<T>::copyOperation(const T* src, uint32_t num) {

    T* dataAlias = getData();
    uint32_t i = 0;

    if (num >= getSize()) {

        for (; i < getSize(); ++i) {
            assignValueTo((dataAlias + i), src[i]);
        }

        for (; i < num; ++i) {
            placeValueTo((dataAlias + i), src[i]);
        }

    } else {

        for (; i < num; ++i) {
            assignValueTo((dataAlias + i), src[i]);
        }
    }

    destruct((dataAlias + i), end());
    proxy.setSize(num);
}


#if ETL_USE_CPP11

template<class T>
typename TypedVectorBase<T>::Iterator TypedVectorBase<T>::insertOperation(ConstIterator position,
                                                                          uint32_t numToInsert,
                                                                          CreatorFunctor&& creatorCall) {

    if (numToInsert > 0) {

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

        while (uninitedInsertPos > dst) {
            --uninitedInsertPos;
            creatorCall(uninitedInsertPos, true);
        }

        dst -= initedInsertNumber;

        while (uninitedInsertPos > dst) {
            --uninitedInsertPos;
            creatorCall(uninitedInsertPos, false);
        }

        proxy.setSize(getSize() + numToInsert);
    }

    return Iterator(position);
}

#else

template<class T>
typename TypedVectorBase<T>::Iterator TypedVectorBase<T>::insertOperation(ConstIterator position,
                                                                          uint32_t numToInsert,
                                                                          const CreatorFunctor& creatorCall) {

    if (numToInsert > 0) {

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

        while (uninitedInsertPos > dst) {
            --uninitedInsertPos;
            creatorCall.call(uninitedInsertPos, true);
        }

        dst -= initedInsertNumber;

        while (uninitedInsertPos > dst) {
            --uninitedInsertPos;
            creatorCall.call(uninitedInsertPos, false);
        }

        proxy.setSize(getSize() + numToInsert);
    }

    return Iterator(position);
}

#endif


template<class T>
void TypedVectorBase<T>::uninitializedCopy(T* src, T* dst, uint32_t num) {

    if (src != dst) {
        for (int i = (num - 1); i >= 0; --i) {              // uninitializedCopy() always copies upwards
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

    if (src != dst) {
        for (int i = (num - 1); i >= 0; --i) {
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

    if (src != dst) {
        for (uint32_t i = 0; i < num; ++i) {
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
    proxy.setSize(0);
}


template<class T>
void TypedVectorBase<T>::destruct(Iterator startPos, Iterator endPos) {

    while (startPos < endPos) {         // operator<() instead of !=() : protection for startPos > endPos cases
        startPos->~T();
        ++startPos;
    }
}


#if ETL_USE_EXCEPTIONS

template<typename T>
T& TypedVectorBase<T>::at(uint32_t ix) {

    if (ix >= getSize()) {
        throw ETL_NAMESPACE::OutOfRangeException();
    }

    return operator[](ix);
}


template<typename T>
const T& TypedVectorBase<T>::at(uint32_t ix) const {

    if (ix >= getSize()) {
        throw ETL_NAMESPACE::OutOfRangeException();
    }

    return operator[](ix);
}

#endif

}

#endif /* __ETL_TYPEDVECTORBASE_H__ */

