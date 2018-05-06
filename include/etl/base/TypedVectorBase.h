/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016 Balazs Toth.

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

#include <etl/etlSupport.h>
#include <etl/base/AVectorBase.h>
#include <etl/base/tools.h>

#if ETL_USE_EXCEPTIONS
#include <etl/ExceptionTypes.h>
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
    template<class C, class A> friend class DynamicSized;

  public:   // types

    typedef T ValueType;
    typedef T& Reference;
    typedef const T& ConstReference;
    typedef T* Pointer;
    typedef const T* ConstPointer;

    typedef Pointer Iterator;
    typedef ConstPointer ConstIterator;

  protected:

#if ETL_USE_CPP11

    using CreateFunc = std::function<void(T*, bool)>;

#endif

    class Creator {
      public:
        virtual void call(T* pos, bool place) const = 0;
    };

    class DefaultCreator : public Creator {
      public:
        virtual void call(T* pos, bool place) const {
            if (place) {
                placeDefaultTo(pos);
            } else {
                assignDefaultTo(pos);
            }
        }
    };

    class CopyCreator : public Creator {
      private:
        const T& ref;

      public:
        explicit CopyCreator(const T& refValue) :
            ref(refValue) {};

        virtual void call(T* pos, bool place) const {
            copyValue(pos, ref, place);
        }
    };

    template<class InputIt>
    class ContCreator : public Creator {
      private:

        const InputIt first;
        mutable InputIt last;
        const uint32_t len;

      public:

        ContCreator(InputIt fst,
                    InputIt lst) :
            first(fst),
            last(lst),
            len((first < last) ? (last - first) : 0) {};

        uint32_t getLength() const {
            return len;
        }

        virtual void call(T* pos, bool place) const {
            if (first < last) {
                --last;
                if (place) {
                    placeValueTo(pos, *last);
                } else {
                    assignValueTo(pos, *last);
                }
            }
        }
    };

  public:   // functions

    Reference operator[](uint32_t ix) {
        return *(static_cast<T*>(getItemPointer(ix)));
    }

    ConstReference operator[](uint32_t ix) const {
        return *(static_cast<const T*>(getItemPointer(ix)));
    }

#if ETL_USE_EXCEPTIONS

    Reference at(uint32_t ix);
    ConstReference at(uint32_t ix) const;

#endif

    Iterator begin() {
        return static_cast<Iterator>(getItemPointer(0));
    }

    ConstIterator begin() const {
        return static_cast<ConstIterator>(getItemPointer(0));
    }

    ConstIterator cbegin() const {
        return this->begin();
    }

    Iterator end() {
        return static_cast<Iterator>(getItemPointer(getSize()));
    }

    ConstIterator end() const {
        return static_cast<ConstIterator>(getItemPointer(getSize()));
    }

    ConstIterator cend() const {
        return this->end();
    }

    Reference front() {
        return *(static_cast<T*>(getItemPointer(0)));
    }

    ConstReference front() const {
        return *(static_cast<T*>(getItemPointer(0)));
    }

    Reference back() {
        return *(static_cast<T*>(getItemPointer(getSize() - 1)));
    }

    ConstReference back() const {
        return *(static_cast<const T*>(getItemPointer(getSize() - 1)));
    }

    T* getData() {
        return static_cast<T*>(getItemPointer(0));
    }

    const T* getData() const {
        return static_cast<const T*>(getItemPointer(0));
    }

    void popFront() {
        erase(begin());
    }

    void popBack() {
        erase(end() - 1);
    }

    Iterator erase(Iterator pos) {
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

    static void assignDefaultTo(T* ptr) {
        *ptr = T();
    }

    static void placeDefaultTo(T* ptr) {
        new (ptr) T();
    }

    static void assignValueTo(T* ptr, const T& value) {
        *ptr = value;
    }

    static void placeValueTo(T* ptr, const T& value) {
        new (ptr) T(value);
    }

    static void copyValue(T* ptr, const T& value, bool place) {
        if (place) {
            placeValueTo(ptr, value);
        } else {
            assignValueTo(ptr, value);
        }
    }

    void copyOperation(T* dst, const T* src, uint32_t num);

#if ETL_USE_CPP11

    static void assignValueTo(T* ptr, T&& value) {
        *ptr = std::move(value);
    }

    static void placeValueTo(T* ptr, T&& value) {
        new (ptr) T(std::move(value));
    }

    static void moveValue(T* ptr, T&& value, bool place) {
        if (place) {
            placeValueTo(ptr, std::move(value));
        } else {
            assignValueTo(ptr, std::move(value));
        }
    }

    static void swapValues(T& lhs, T& rhs) {
        T tmp(std::move(lhs));
        lhs = std::move(rhs);
        rhs = std::move(tmp);
    }

    void moveOperation(T* dst, T* src, uint32_t num);

#else

    static void swapValues(T& lhs, T& rhs) {
        T tmp(lhs);
        lhs = rhs;
        rhs = tmp;
    }

#endif

    static void uninitializedCopy(T* src, T* dst, uint32_t num);
    static void initializedCopyUp(T* src, T* dst, uint32_t num);
    static void initializedCopyDown(T* src, T* dst, uint32_t num);

    static void destruct(Iterator startPos, Iterator endPos);

    Iterator insertOperation(ConstIterator position, uint32_t num, const Creator& creatorCall);

#if ETL_USE_CPP11

    Iterator insertOperation(ConstIterator position, uint32_t num, CreateFunc&& creatorCall);

#endif

    void swapElements(TypedVectorBase& other);

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
void TypedVectorBase<T>::copyOperation(T* dst, const T* src, uint32_t num) {

    T* dataAlias = getData();

    if (dst >= dataAlias) {

        uint32_t totalNum = dst - dataAlias + num;

        for (uint32_t i = 0; i < num; ++i) {
            copyValue(dst, src[i], ((dst - dataAlias) >= this->getSize()));
            ++dst;
        }

        destruct(dst, end());
        proxy.setSize(totalNum);
    }
}


template<class T>
typename TypedVectorBase<T>::Iterator TypedVectorBase<T>::insertOperation(ConstIterator position,
                                                                          uint32_t numToInsert,
                                                                          const Creator& creatorCall) {

    if (numToInsert > 0) {

        uint32_t distanceFromEnd = end() - position;

        uint32_t uninitedCopyCnt = (distanceFromEnd >= numToInsert) ? numToInsert : distanceFromEnd;
        uint32_t initedCopyCnt = (distanceFromEnd >= numToInsert) ? (distanceFromEnd - numToInsert) : 0;
        uint32_t uninitedInsertCnt = (distanceFromEnd >= numToInsert) ? 0 : (numToInsert - distanceFromEnd);
        uint32_t initedInsertCnt = uninitedCopyCnt;

        T* src = end() - uninitedCopyCnt;
        T* dst = end() + numToInsert - uninitedCopyCnt;

        uninitializedCopy(src, dst, uninitedCopyCnt);

        src -= initedCopyCnt;
        dst -= initedCopyCnt;

        initializedCopyUp(src, dst, initedCopyCnt);

        T* uninitedInsertPos = dst;
        dst -= uninitedInsertCnt;

        while (uninitedInsertPos > dst) {
            --uninitedInsertPos;
            creatorCall.call(uninitedInsertPos, true);
        }

        dst -= initedInsertCnt;

        while (uninitedInsertPos > dst) {
            --uninitedInsertPos;
            creatorCall.call(uninitedInsertPos, false);
        }

        proxy.setSize(getSize() + numToInsert);
    }

    return Iterator(position);
}


#if ETL_USE_CPP11

template<class T>
typename TypedVectorBase<T>::Iterator TypedVectorBase<T>::insertOperation(ConstIterator position,
                                                                          uint32_t numToInsert,
                                                                          CreateFunc&& creatorCall) {

    if (numToInsert > 0) {

        uint32_t distanceFromEnd = end() - position;

        uint32_t uninitedCopyCnt = (distanceFromEnd >= numToInsert) ? numToInsert : distanceFromEnd;
        uint32_t initedCopyCnt = (distanceFromEnd >= numToInsert) ? (distanceFromEnd - numToInsert) : 0;
        uint32_t uninitedInsertCnt = (distanceFromEnd >= numToInsert) ? 0 : (numToInsert - distanceFromEnd);
        uint32_t initedInsertCnt = uninitedCopyCnt;

        T* src = end() - uninitedCopyCnt;
        T* dst = end() + numToInsert - uninitedCopyCnt;

        uninitializedCopy(src, dst, uninitedCopyCnt);

        src -= initedCopyCnt;
        dst -= initedCopyCnt;

        initializedCopyUp(src, dst, initedCopyCnt);

        T* uninitedInsertPos = dst;
        dst -= uninitedInsertCnt;

        while (uninitedInsertPos > dst) {
            --uninitedInsertPos;
            creatorCall(uninitedInsertPos, true);
        }

        dst -= initedInsertCnt;

        while (uninitedInsertPos > dst) {
            --uninitedInsertPos;
            creatorCall(uninitedInsertPos, false);
        }

        proxy.setSize(getSize() + numToInsert);
    }

    return Iterator(position);
}


template<class T>
void TypedVectorBase<T>::moveOperation(T* dst, T* src, uint32_t num) {

    T* dataAlias = getData();

    if (dst >= dataAlias) {

        uint32_t totalNum = dst - dataAlias + num;

        for (uint32_t i = 0; i < num; ++i) {
            moveValue(dst, std::move(src[i]), ((dst - dataAlias) >= this->getSize()));
            ++dst;
        }

        destruct(dst, end());
        proxy.setSize(totalNum);
    }
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


template<class T>
void TypedVectorBase<T>::swapElements(TypedVectorBase<T>& other) {

    const SizeDiff diff(*this, other);

    for (uint32_t i = 0; i < diff.common; ++i) {
        this->swapValues(this->operator[](i), other[i]);
    }

    if (diff.rGreaterWith > 0) {

#if ETL_USE_CPP11
        this->moveOperation(&this->operator[](diff.common), &other[diff.common], diff.rGreaterWith);
#else
        this->copyOperation(&this->operator[](diff.common), &other[diff.common], diff.rGreaterWith);
#endif
        other.destruct(&other[diff.common], other.end());
        other.proxy.setSize(diff.common);

    } else if (diff.lGreaterWith > 0) {

#if ETL_USE_CPP11
        other.moveOperation(&other[diff.common], &this->operator[](diff.common), diff.lGreaterWith);
#else
        other.copyOperation(&other[diff.common], &this->operator[](diff.common), diff.lGreaterWith);
#endif
        this->destruct(&this->operator[](diff.common), this->end());
        this->proxy.setSize(diff.common);
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
ConstReference TypedVectorBase<T>::at(uint32_t ix) const {

    if (ix >= getSize()) {
        throw ETL_NAMESPACE::OutOfRangeException();
    }

    return operator[](ix);
}

#endif

}

#endif /* __ETL_TYPEDVECTORBASE_H__ */

