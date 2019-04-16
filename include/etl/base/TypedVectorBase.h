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
#include <iterator>

#if ETL_USE_CPP11
#include <functional>
#include <initializer_list>
#endif

namespace ETL_NAMESPACE {

template<class> class StaticSized;
template<class, class> class DynamicSized;


template<class T>
class TypedVectorBase : public AVectorBase {

    friend class StaticSized<TypedVectorBase>;
    template<class C, class A> friend class DynamicSized;

  public:   // types

    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef pointer iterator;
    typedef const_pointer const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  protected:

    using CreateFunc = std::function<void(pointer, uint32_t, bool)>;

    class DefaultCreator {
      public:
        void operator()(pointer pos, uint32_t cnt, bool place) const {
            for (uint32_t i = 0; i < cnt; ++i) {
                defaultValue(pos + i, place);
            }
        }
    };

    class CopyCreator {
      private:
        const_reference ref;
      public:
        explicit CopyCreator(const_reference refValue) :
            ref(refValue) {};
        void operator()(pointer pos, uint32_t cnt, bool place) const {
            for (uint32_t i = 0; i < cnt; ++i) {
                copyValue(pos + i, ref, place);
            }
        }
    };

    template<class InputIt>
    class ContCreator {
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
        void operator()(pointer pos, uint32_t cnt, bool place) const {
            for (int32_t i = (static_cast<int32_t>(cnt) - 1); i >= 0; --i) {
                if (first < last) {
                    --last;
                    copyValue(pos + i, *last, place);
                }
            }
        }
    };

  public:   // functions

    /// \name Element access
    /// \{
    reference operator[](uint32_t ix) {
        return *(static_cast<pointer>(getItemPointer(ix)));
    }

    const_reference operator[](uint32_t ix) const {
        return *(static_cast<const_pointer>(getItemPointer(ix)));
    }

#if ETL_USE_EXCEPTIONS

    reference at(uint32_t ix);
    const_reference at(uint32_t ix) const;

#endif

    reference front() {
        return *(static_cast<pointer>(getItemPointer(0)));
    }

    const_reference front() const {
        return *(static_cast<const_pointer>(getItemPointer(0)));
    }

    reference back() {
        return *(static_cast<pointer>(getItemPointer(size() - 1)));
    }

    const_reference back() const {
        return *(static_cast<const_pointer>(getItemPointer(size() - 1)));
    }

    pointer data() {
        return static_cast<pointer>(getItemPointer(0));
    }

    const_pointer data() const {
        return static_cast<const_pointer>(getItemPointer(0));
    }
    /// \}

    /// \name Iterators
    /// \{
    iterator begin() {
        return static_cast<iterator>(getItemPointer(0));
    }

    const_iterator begin() const {
        return static_cast<const_iterator>(getItemPointer(0));
    }

    const_iterator cbegin() const {
        return this->begin();
    }

    iterator end() {
        return static_cast<iterator>(getItemPointer(size()));
    }

    const_iterator end() const {
        return static_cast<const_iterator>(getItemPointer(size()));
    }

    const_iterator cend() const {
        return this->end();
    }

    reverse_iterator rbegin() {
        return reverse_iterator(this->end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(this->end());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(this->cend());
    }

    reverse_iterator rend() {
        return reverse_iterator(this->begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(this->begin());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(this->cbegin());
    }
    /// \}

    /// \name Modifiers
    /// \{
    void clear();
    iterator erase(iterator first, iterator last);

    iterator erase(iterator pos) {
        iterator next = pos;
        ++next;
        return erase(pos, next);
    }

    void pop_front() {
        erase(begin());
    }

    void pop_back() {
        erase(end() - 1);
    }
    /// \}

  protected:

    TypedVectorBase() :
        AVectorBase(sizeof(T)) {};

    ~TypedVectorBase() {
        clear();
    }

    static void assignDefaultTo(pointer ptr) {
        *ptr = T();
    }

    static void placeDefaultTo(pointer ptr) {
        new (ptr) T();
    }

    static void defaultValue(pointer ptr, bool place) {
        if (place) {
            placeDefaultTo(ptr);
        } else {
            assignDefaultTo(ptr);
        }
    }

    static void assignValueTo(pointer ptr, const_reference value) {
        *ptr = value;
    }

    static void placeValueTo(pointer ptr, const_reference value) {
        new (ptr) T(value);
    }

    static void copyValue(pointer ptr, const_reference value, bool place) {
        if (place) {
            placeValueTo(ptr, value);
        } else {
            assignValueTo(ptr, value);
        }
    }

    void copyOperation(pointer dst, const_pointer src, uint32_t num);
    void moveOperation(pointer dst, pointer src, uint32_t num);

    static void assignValueTo(pointer ptr, value_type&& value) {
        *ptr = std::move(value);
    }

    static void placeValueTo(pointer ptr, value_type&& value) {
        new (ptr) T(std::move(value));
    }

    static void moveValue(pointer ptr, value_type&& value, bool place) {
        if (place) {
            placeValueTo(ptr, std::move(value));
        } else {
            assignValueTo(ptr, std::move(value));
        }
    }

    static void swapValues(reference lhs, reference rhs) {
        value_type tmp(std::move(lhs));
        lhs = std::move(rhs);
        rhs = std::move(tmp);
    }

    static void uninitializedCopy(pointer src, pointer dst, uint32_t num);
    static void initializedCopyUp(pointer src, pointer dst, uint32_t num);
    static void initializedCopyDown(pointer src, pointer dst, uint32_t num);

    static void destruct(iterator startPos, iterator endPos);

    template<class CR>
    iterator insertOperation(const_iterator position, uint32_t num, const CR& creatorCall);

    void swapElements(TypedVectorBase& other);

};


template<class T>
typename TypedVectorBase<T>::iterator TypedVectorBase<T>::erase(iterator first, iterator last) {

    int numToErase = last - first;
    iterator itAfterDeleted = first;

    if (numToErase > 0) {

        uint32_t numToMove = end() - last;
        initializedCopyDown(&*last, &*first, numToMove);

        first += numToMove;
        destruct(first, end());

        proxy.setSize(size() - numToErase);
    }

    return itAfterDeleted;
}


template<class T>
void TypedVectorBase<T>::copyOperation(pointer dst, const_pointer src, uint32_t num) {

    pointer dataAlias = data();

    if (dst >= dataAlias) {

        uint32_t totalNum = static_cast<uint32_t>(dst - dataAlias) + num;

        for (uint32_t i = 0; i < num; ++i) {
            copyValue(dst, src[i], (static_cast<uint32_t>(dst - dataAlias) >= this->size()));
            ++dst;
        }

        destruct(dst, end());
        proxy.setSize(totalNum);
    }
}


template<class T>
template<class CR>
typename TypedVectorBase<T>::iterator TypedVectorBase<T>::insertOperation(const_iterator position,
                                                                          uint32_t numToInsert,
                                                                          const CR& creatorCall) {

    if (numToInsert > 0) {

        uint32_t distanceFromEnd = end() - position;

        uint32_t uninitedCopyCnt = (distanceFromEnd >= numToInsert) ? numToInsert : distanceFromEnd;
        uint32_t initedCopyCnt = (distanceFromEnd >= numToInsert) ? (distanceFromEnd - numToInsert) : 0;
        uint32_t uninitedInsertCnt = (distanceFromEnd >= numToInsert) ? 0 : (numToInsert - distanceFromEnd);
        uint32_t initedInsertCnt = uninitedCopyCnt;

        pointer src = end() - uninitedCopyCnt;
        pointer dst = end() + numToInsert - uninitedCopyCnt;

        uninitializedCopy(src, dst, uninitedCopyCnt);

        src -= initedCopyCnt;
        dst -= initedCopyCnt;

        initializedCopyUp(src, dst, initedCopyCnt);

        if (uninitedInsertCnt > 0) {
            dst -= uninitedInsertCnt;
            creatorCall(dst, uninitedInsertCnt, true);
        }

        if (initedInsertCnt > 0) {
            dst -= initedInsertCnt;
            creatorCall(dst, initedInsertCnt, false);
        }

        proxy.setSize(size() + numToInsert);
    }

    return iterator(position);
}


template<class T>
void TypedVectorBase<T>::moveOperation(pointer dst, pointer src, uint32_t num) {

    pointer dataAlias = data();

    if (dst >= dataAlias) {

        uint32_t totalNum = dst - dataAlias + num;

        for (uint32_t i = 0; i < num; ++i) {
            moveValue(dst, std::move(src[i]), ((dst - dataAlias) >= this->size()));
            ++dst;
        }

        destruct(dst, end());
        proxy.setSize(totalNum);
    }
}


template<class T>
void TypedVectorBase<T>::uninitializedCopy(pointer src, pointer dst, uint32_t num) {

    if (src != dst) {
        for (int i = (num - 1); i >= 0; --i) {              // uninitializedCopy() always copies upwards
            placeValueTo((dst + i), std::move(src[i]));     // Placement new, move constructor
        }
    }
}


template<class T>
void TypedVectorBase<T>::initializedCopyUp(pointer src, pointer dst, uint32_t num) {

    if (src != dst) {
        for (int i = (num - 1); i >= 0; --i) {
            assignValueTo((dst + i), std::move(src[i]));
        }
    }
}


template<class T>
void TypedVectorBase<T>::initializedCopyDown(pointer src, pointer dst, uint32_t num) {

    if (src != dst) {
        for (uint32_t i = 0; i < num; ++i) {
            assignValueTo((dst + i), std::move(src[i]));
        }
    }
}


template<class T>
void TypedVectorBase<T>::clear() {

    destruct(begin(), end());
    proxy.setSize(0);
}


template<class T>
void TypedVectorBase<T>::destruct(iterator startPos, iterator endPos) {

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

        this->moveOperation(&this->operator[](diff.common), &other[diff.common], diff.rGreaterWith);
        other.destruct(&other[diff.common], other.end());
        other.proxy.setSize(diff.common);

    } else if (diff.lGreaterWith > 0) {

        other.moveOperation(&other[diff.common], &this->operator[](diff.common), diff.lGreaterWith);
        this->destruct(&this->operator[](diff.common), this->end());
        this->proxy.setSize(diff.common);
    }
}


#if ETL_USE_EXCEPTIONS

template<typename T>
typename TypedVectorBase<T>::reference TypedVectorBase<T>::at(uint32_t ix) {

    if (ix >= size()) {
        throw ETL_NAMESPACE::OutOfRangeException();
    }

    return operator[](ix);
}


template<typename T>
typename TypedVectorBase<T>::const_reference TypedVectorBase<T>::at(uint32_t ix) const {

    if (ix >= size()) {
        throw ETL_NAMESPACE::OutOfRangeException();
    }

    return operator[](ix);
}

#endif

}

#endif /* __ETL_TYPEDVECTORBASE_H__ */

