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
#include <functional>

namespace ETL_NAMESPACE {

template<class> class StaticSized;
template<class, class> class DynamicSized;

namespace Detail {


template<class T>
class TypedVectorBase : public AVectorBase {

    friend class StaticSized<TypedVectorBase>;
    template<class, class> friend class DynamicSized;

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
        void operator()(pointer pos, uint32_t cnt, bool place) const
        noexcept(noexcept(TypedVectorBase::defaultValue(pos, place))) {
            for (uint32_t i = 0; i < cnt; ++i) {
                TypedVectorBase::defaultValue(pos + i, place);
            }
        }
    };

    class CopyCreator {
      private:
        const_reference ref;
      public:
        explicit CopyCreator(const_reference refValue) noexcept :
            ref(refValue) {};
        void operator()(pointer pos, uint32_t cnt, bool place) const
        noexcept(noexcept(TypedVectorBase::copyValue(pos, ref, place))) {
            for (uint32_t i = 0; i < cnt; ++i) {
                TypedVectorBase::copyValue(pos + i, ref, place);
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
                    InputIt lst) noexcept :
            first(fst),
            last(lst),
            len((first < last) ? (last - first) : 0) {};
        uint32_t getLength() const noexcept {
            return len;
        }
        void operator()(pointer pos, uint32_t cnt, bool place) const
        noexcept(noexcept(TypedVectorBase::copyValue(pos, *last, place))) {
            for (int32_t i = (static_cast<int32_t>(cnt) - 1); i >= 0; --i) {
                if (first < last) {
                    --last;
                    TypedVectorBase::copyValue(pos + i, *last, place);
                }
            }
        }
    };

  public:   // functions

    /// \name Element access
    /// \{
    reference operator[](uint32_t ix) noexcept {
        return *(static_cast<pointer>(getItemPointer(ix)));
    }

    const_reference operator[](uint32_t ix) const noexcept {
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

    pointer data() noexcept {
        return static_cast<pointer>(getItemPointer(0));
    }

    const_pointer data() const noexcept {
        return static_cast<const_pointer>(getItemPointer(0));
    }
    /// \}

    /// \name Iterators
    /// \{
    iterator begin() noexcept {
        return static_cast<iterator>(getItemPointer(0));
    }

    const_iterator begin() const noexcept {
        return static_cast<const_iterator>(getItemPointer(0));
    }

    const_iterator cbegin() const noexcept {
        return this->begin();
    }

    iterator end() noexcept {
        return static_cast<iterator>(getItemPointer(size()));
    }

    const_iterator end() const noexcept {
        return static_cast<const_iterator>(getItemPointer(size()));
    }

    const_iterator cend() const noexcept {
        return this->end();
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(this->end());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(this->end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(this->cend());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(this->begin());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(this->begin());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(this->cbegin());
    }
    /// \}

    /// \name Modifiers
    /// \{
    void clear() noexcept;
    iterator erase(iterator first, iterator last) noexcept(std::is_nothrow_move_assignable<T>::value);

    iterator erase(iterator pos) noexcept(std::is_nothrow_move_assignable<T>::value) {
        iterator next = pos;
        ++next;
        return erase(pos, next);
    }

    void pop_front() noexcept(std::is_nothrow_move_assignable<T>::value) {
        erase(begin());
    }

    void pop_back() noexcept(std::is_nothrow_move_assignable<T>::value) {
        erase(end() - 1);
    }
    /// \}

  protected:

    TypedVectorBase() noexcept :
        AVectorBase(sizeof(T)) {};

    ~TypedVectorBase() {
        clear();
    }

    void copyOperation(pointer dst, const_pointer src, uint32_t num)
    noexcept(std::is_nothrow_copy_assignable<T>::value && std::is_nothrow_copy_constructible<T>::value);

    void moveOperation(pointer dst, pointer src, uint32_t num)
    noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value);

    static void assignDefaultTo(pointer ptr)
    noexcept(std::is_nothrow_default_constructible<T>::value && std::is_nothrow_move_assignable<T>::value) {
        *ptr = T();
    }

    static void placeDefaultTo(pointer ptr) noexcept(std::is_nothrow_default_constructible<T>::value) {
        new (ptr) T();
    }

    static void defaultValue(pointer ptr, bool place)
    noexcept(noexcept(placeDefaultTo(ptr)) && noexcept(assignDefaultTo(ptr))) {
        if (place) {
            placeDefaultTo(ptr);
        } else {
            assignDefaultTo(ptr);
        }
    }

    static void assignValueTo(pointer ptr, const_reference value) noexcept(std::is_nothrow_copy_assignable<T>::value) {
        *ptr = value;
    }

    static void placeValueTo(pointer ptr, const_reference value) noexcept(std::is_nothrow_copy_constructible<T>::value) {
        new (ptr) T(value);
    }

    static void copyValue(pointer ptr, const_reference value, bool place)
    noexcept(noexcept(placeValueTo(ptr, value)) && noexcept(assignValueTo(ptr, value))) {
        if (place) {
            placeValueTo(ptr, value);
        } else {
            assignValueTo(ptr, value);
        }
    }

    static void assignValueTo(pointer ptr, value_type&& value) noexcept(std::is_nothrow_move_assignable<T>::value) {
        *ptr = std::move(value);
    }

    static void placeValueTo(pointer ptr, value_type&& value) noexcept(std::is_nothrow_move_constructible<T>::value) {
        new (ptr) T(std::move(value));
    }

    static void moveValue(pointer ptr, value_type&& value, bool place)
    noexcept(noexcept(placeValueTo(ptr, std::move(value))) && noexcept(assignValueTo(ptr, std::move(value)))) {
        if (place) {
            placeValueTo(ptr, std::move(value));
        } else {
            assignValueTo(ptr, std::move(value));
        }
    }

    static void swapValues(reference lhs, reference rhs)
    noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value) {
        value_type tmp(std::move(lhs));
        lhs = std::move(rhs);
        rhs = std::move(tmp);
    }

    static void uninitializedMove(pointer src, pointer dst, uint32_t num)
    noexcept(std::is_nothrow_move_constructible<T>::value);

    static void initializedMoveUp(pointer src, pointer dst, uint32_t num)
    noexcept(std::is_nothrow_move_assignable<T>::value);

    static void initializedMoveDown(pointer src, pointer dst, uint32_t num)
    noexcept(std::is_nothrow_move_assignable<T>::value);

    static void destruct(iterator startPos, iterator endPos) noexcept;

    template<class CR>
    iterator insertOperation(const_iterator position, uint32_t num, const CR& creatorCall)
    noexcept(noexcept(creatorCall(nullptr, 1, true)) &&
             std::is_nothrow_move_assignable<T>::value &&
             std::is_nothrow_move_constructible<T>::value);

    void swapElements(TypedVectorBase& other)
    noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value);

};


template<class T>
auto TypedVectorBase<T>::erase(iterator first, iterator last)
noexcept(std::is_nothrow_move_assignable<T>::value) -> iterator {

    int numToErase = last - first;
    iterator itAfterDeleted = first;

    if (numToErase > 0) {

        uint32_t numToMove = end() - last;
        initializedMoveDown(&*last, &*first, numToMove);

        first += numToMove;
        destruct(first, end());

        proxy.setSize(size() - numToErase);
    }

    return itAfterDeleted;
}


template<class T>
void TypedVectorBase<T>::copyOperation(pointer dst, const_pointer src, uint32_t num)
noexcept(std::is_nothrow_copy_assignable<T>::value && std::is_nothrow_copy_constructible<T>::value) {

    pointer dataAlias = data();

    if (dst >= dataAlias) {

        uint32_t totalNum = static_cast<uint32_t>(dst - dataAlias) + num;

        for (uint32_t i = 0; i < num; ++i) {
            TypedVectorBase::copyValue(dst, src[i], (static_cast<uint32_t>(dst - dataAlias) >= this->size()));
            ++dst;
        }

        TypedVectorBase::destruct(dst, end());
        proxy.setSize(totalNum);
    }
}


template<class T>
template<class CR>
auto TypedVectorBase<T>::insertOperation(const_iterator position,
                                         uint32_t numToInsert,
                                         const CR& creatorCall)
noexcept(noexcept(creatorCall(nullptr, 1, true))&&
         std::is_nothrow_move_assignable<T>::value &&
         std::is_nothrow_move_constructible<T>::value) -> iterator {

    if (numToInsert > 0) {

        uint32_t distanceFromEnd = end() - position;

        uint32_t uninitedCopyCnt = (distanceFromEnd >= numToInsert) ? numToInsert : distanceFromEnd;
        uint32_t initedCopyCnt = (distanceFromEnd >= numToInsert) ? (distanceFromEnd - numToInsert) : 0;
        uint32_t uninitedInsertCnt = (distanceFromEnd >= numToInsert) ? 0 : (numToInsert - distanceFromEnd);
        uint32_t initedInsertCnt = uninitedCopyCnt;

        pointer src = end() - uninitedCopyCnt;
        pointer dst = end() + numToInsert - uninitedCopyCnt;

        uninitializedMove(src, dst, uninitedCopyCnt);

        src -= initedCopyCnt;
        dst -= initedCopyCnt;

        initializedMoveUp(src, dst, initedCopyCnt);

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
void TypedVectorBase<T>::moveOperation(pointer dst, pointer src, uint32_t num)
noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value) {

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
void TypedVectorBase<T>::uninitializedMove(pointer src, pointer dst, uint32_t num)
noexcept(std::is_nothrow_move_constructible<T>::value) {

    if (src != dst) {
        for (int i = (num - 1); i >= 0; --i) {              // always moves upwards
            placeValueTo((dst + i), std::move(src[i]));
        }
    }
}


template<class T>
void TypedVectorBase<T>::initializedMoveUp(pointer src, pointer dst, uint32_t num)
noexcept(std::is_nothrow_move_assignable<T>::value) {

    if (src != dst) {
        for (int i = (num - 1); i >= 0; --i) {
            assignValueTo((dst + i), std::move(src[i]));
        }
    }
}


template<class T>
void TypedVectorBase<T>::initializedMoveDown(pointer src, pointer dst, uint32_t num)
noexcept(std::is_nothrow_move_assignable<T>::value) {

    if (src != dst) {
        for (uint32_t i = 0; i < num; ++i) {
            assignValueTo((dst + i), std::move(src[i]));
        }
    }
}


template<class T>
void TypedVectorBase<T>::clear() noexcept {

    destruct(begin(), end());
    proxy.setSize(0);
}


template<class T>
void TypedVectorBase<T>::destruct(iterator startPos, iterator endPos) noexcept {

    while (startPos < endPos) {
        startPos->~T();
        ++startPos;
    }
}


template<class T>
void TypedVectorBase<T>::swapElements(TypedVectorBase<T>& other)
noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value) {

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
}

#endif /* __ETL_TYPEDVECTORBASE_H__ */

