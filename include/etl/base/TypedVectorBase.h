/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2022 Balazs Toth.

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

#include <etl/base/AVectorBase.h>
#include <etl/base/tools.h>
#include <etl/etlSupport.h>

#if ETL_USE_EXCEPTIONS
#include <etl/ExceptionTypes.h>
#endif

#undef min
#undef max

#include <cstddef>
#include <functional>
#include <iterator>
#include <new>
#include <utility>


#ifndef ETL_VEC_INSERT_OP_DIR_UP
#define ETL_VEC_INSERT_OP_DIR_UP 1
#endif

namespace ETL_NAMESPACE {

template<class>
class StaticSized;
template<class, class, bool>
class DynamicSized;

namespace Detail {

template<class>
struct TrivialElementOps;
template<class>
struct NonTrivialElementOps;

template<class T>
class TypedVectorBase : public AVectorBase {

    friend class StaticSized<TypedVectorBase>;
    template<class, class, bool>
    friend class DynamicSized;

  public:  // types

    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

    typedef pointer iterator;
    typedef const_pointer const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef std::uint32_t size_type;

  protected:

    using CreateFunc = std::function<void(pointer, size_type, bool)>;

    class DefaultCreator;
    class CopyCreator;

    template<class InputIt>
    class ContCreator;

  public:  // functions

    /// \name Element access
    /// \{

    reference operator[](size_type ix) noexcept {
        ETL_ASSERT(ix < size());
        return *getItemPointer(ix);
    }

    const_reference operator[](size_type ix) const noexcept {
        ETL_ASSERT(ix < size());
        return *getConstItemPointer(ix);
    }

#if ETL_USE_EXCEPTIONS

    reference at(size_type ix);
    const_reference at(size_type ix) const;

#endif

    reference front() {
        ETL_ASSERT(!empty());
        return *getItemPointer(0);
    }

    const_reference front() const {
        ETL_ASSERT(!empty());
        return *getConstItemPointer(0);
    }

    reference back() {
        ETL_ASSERT(!empty());
        return *getItemPointer(size() - 1U);
    }

    const_reference back() const {
        ETL_ASSERT(!empty());
        return *getConstItemPointer(size() - 1U);
    }

    pointer data() noexcept {
        return getItemPointer(0);
    }

    const_pointer data() const noexcept {
        return getConstItemPointer(0);
    }
    /// \}

    /// \name Iterators
    /// \{
    iterator begin() noexcept {
        return getIterator(0);
    }

    const_iterator begin() const noexcept {
        return getConstIterator(0);
    }

    const_iterator cbegin() const noexcept {
        return this->begin();
    }

    iterator end() noexcept {
        return getIterator(size());
    }

    const_iterator end() const noexcept {
        return getConstIterator(size());
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
    void clear() noexcept(is_nothrow_destructible<T>::value);
    iterator erase(iterator first, iterator last) noexcept(is_nothrow_move_assignable<T>::value);

    iterator erase(iterator pos) noexcept(is_nothrow_move_assignable<T>::value) {
        iterator next = pos;
        ++next;
        return erase(pos, next);
    }

    void pop_front() noexcept(is_nothrow_move_assignable<T>::value) {
        erase(begin());
    }

    void pop_back() noexcept(is_nothrow_move_assignable<T>::value) {
        erase(end() - 1);
    }
    /// \}

    AVectorBase::Proxy& getProxy() {
        return this->proxy;
    }

    const AVectorBase::Proxy& getProxy() const {
        return this->proxy;
    }

    static void swapValues(reference lhs, reference rhs) noexcept(
        is_nothrow_move_assignable<T>::value&& is_nothrow_move_constructible<T>::value) {
        using std::swap;
        swap(lhs, rhs);
    }

    template<class V = T>
    static enable_if_t<std::is_trivial<V>::value, TrivialElementOps<V>> ops() {
        return TrivialElementOps<V> {};
    }

    template<class V = T>
    static enable_if_t<!std::is_trivial<V>::value, NonTrivialElementOps<V>> ops() {
        return NonTrivialElementOps<V> {};
    }

  protected:

    TypedVectorBase() noexcept :
        AVectorBase(sizeof(T)) {};

    ~TypedVectorBase() noexcept(is_nothrow_destructible<T>::value) {
        clear();
    }

    pointer getItemPointer(uint32_t ix) noexcept {
        return static_cast<pointer>(AVectorBase::getItemPointer(ix));
    }

    const_pointer getConstItemPointer(uint32_t ix) const noexcept {
        return static_cast<const_pointer>(AVectorBase::getItemPointer(ix));
    }

    iterator getIterator(uint32_t ix) noexcept {
        return static_cast<iterator>(getItemPointer(ix));
    }

    const_iterator getConstIterator(uint32_t ix) const noexcept {
        return static_cast<const_iterator>(getConstItemPointer(ix));
    }

    void copyOperation(pointer dst, const_pointer src, size_type num) noexcept(
        is_nothrow_copy_assignable<T>::value&& is_nothrow_copy_constructible<T>::value);

    void moveFromOther(pointer dst, pointer src, size_type num) noexcept(
        is_nothrow_move_assignable<T>::value&& is_nothrow_move_constructible<T>::value);

    template<class CR>
    iterator
    insertOperation(const_iterator position,
                    size_type num,
                    const CR& creatorCall) noexcept(noexcept(creatorCall(nullptr, 1, true))
                                                    && is_nothrow_move_assignable<T>::value
                                                    && is_nothrow_move_constructible<T>::value);

    template<class CR>
    iterator
    insertOneOperation(const_iterator position,
                       const CR& creatorCall) noexcept(noexcept(creatorCall(nullptr, true))
                                                       && is_nothrow_move_assignable<T>::value
                                                       && is_nothrow_move_constructible<T>::value);

    template<typename InputIt>
    iterator insertRangeOperation(const_iterator position, InputIt first, InputIt last) noexcept(
        is_nothrow_move_assignable<T>::value&& is_nothrow_move_constructible<T>::value);

    void swapNeighbourRanges(size_type toEndStart, size_type toMidStart) noexcept(
        is_nothrow_move_assignable<T>::value&& is_nothrow_move_constructible<T>::value);

    void swapElements(TypedVectorBase& other) noexcept(
        is_nothrow_move_assignable<T>::value&& is_nothrow_move_constructible<T>::value);

    static void destruct(iterator startPos,
                         iterator endPos) noexcept(noexcept(ops().destruct(startPos, endPos))) {
        ops().destruct(startPos, endPos);
    }
};


template<class T>
struct TrivialElementOps {

    static_assert(std::is_trivial<T>::value, "T is not trivial");
    static_assert(std::is_nothrow_default_constructible<T>::value, "T nothrow contract violated");
    static_assert(std::is_nothrow_copy_constructible<T>::value, "T nothrow contract violated");
    static_assert(std::is_nothrow_copy_assignable<T>::value, "T nothrow contract violated");
    static_assert(std::is_nothrow_destructible<T>::value, "T nothrow contract violated");

    using size_type = typename TypedVectorBase<T>::size_type;
    using value_type = typename TypedVectorBase<T>::value_type;
    using pointer = typename TypedVectorBase<T>::pointer;
    using const_pointer = typename TypedVectorBase<T>::const_pointer;
    using reference = typename TypedVectorBase<T>::reference;
    using const_reference = typename TypedVectorBase<T>::const_reference;

    void moveWithAssignment(pointer src,
                            pointer dst,
                            size_type num) noexcept {
        if ((src != dst) && (num > 0U)) {
            auto srcEnd = src + num;
            auto dstEnd = dst + num;
            ETL_ASSERT((dstEnd <= src) || (srcEnd < dstEnd));
            std::copy_backward(src, srcEnd, dstEnd);
        }
    }

    void moveWithPlacement(pointer src,
                           pointer dst,
                           size_type num) noexcept {
        moveWithAssignment(src, dst, num);
    }

    void moveDown(pointer src,
                  pointer dst,
                  size_type num) noexcept {
        if ((src != dst) && (num > 0U)) {
            auto srcEnd = src + num;
            ETL_ASSERT((dst < src) || (srcEnd <= dst));
            std::copy(src, srcEnd, dst);
        }
    }

    void assignDefaultTo(pointer ptr) noexcept {
        *ptr = T {};
    }

    void placeDefaultTo(pointer ptr) noexcept {
        assignDefaultTo(ptr);
    }

    void defaultValue(pointer ptr, bool place) {
        (void)place;
        assignDefaultTo(ptr);
    }

    void assignValueTo(pointer ptr,
                       const_reference value) noexcept {
        *ptr = value;
    }

    void placeValueTo(pointer ptr,
                      const_reference value) noexcept {
        assignValueTo(ptr, value);
    }

    void copyValue(pointer ptr, const_reference value, bool place) noexcept {
        (void)place;
        assignValueTo(ptr, value);
    }

    void moveValue(pointer ptr, const_reference value, bool place) noexcept {
        (void)place;
        assignValueTo(ptr, value);
    }

    void moveOperation(pointer src,
                       pointer dst,
                       size_type num,
                       bool place) noexcept {
        (void)place;
        moveWithAssignment(src, dst, num);
    }

    void destruct(pointer startPos,
                  pointer endPos) noexcept {
        (void)startPos;
        (void)endPos;
    }
};


template<class T>
struct NonTrivialElementOps {
    // static_assert(!std::is_trivially_copyable<T>::value, "T is trivially copyable");

    using size_type = typename TypedVectorBase<T>::size_type;
    using value_type = typename TypedVectorBase<T>::value_type;
    using pointer = typename TypedVectorBase<T>::pointer;
    using const_pointer = typename TypedVectorBase<T>::const_pointer;
    using reference = typename TypedVectorBase<T>::reference;
    using const_reference = typename TypedVectorBase<T>::const_reference;

    void moveWithPlacement(pointer src,
                           pointer dst,
                           size_type num) noexcept(is_nothrow_move_constructible<T>::value) {
        if ((src != dst) && (num > 0U)) {
            for (int i = (num - 1); i >= 0; --i) {  // move upwards
                placeValueTo((dst + i), std::move(src[i]));
            }
        }
    }

    void moveWithAssignment(pointer src,
                            pointer dst,
                            size_type num) noexcept(is_nothrow_move_assignable<T>::value) {
        if ((src != dst) && (num > 0U)) {
            for (int i = (num - 1); i >= 0; --i) {  // move upwards
                assignValueTo((dst + i), std::move(src[i]));
            }
        }
    }

    void moveDown(pointer src,
                  pointer dst,
                  size_type num) noexcept(is_nothrow_move_assignable<T>::value) {
        if ((src != dst) && (num > 0U)) {
            for (size_type i = 0; i < num; ++i) {
                assignValueTo((dst + i), std::move(src[i]));
            }
        }
    }

    void assignDefaultTo(pointer ptr) noexcept(
        is_nothrow_default_constructible<T>::value&& is_nothrow_move_assignable<T>::value) {
        *ptr = T();
    }

    void placeDefaultTo(pointer ptr) noexcept(is_nothrow_default_constructible<T>::value) {
        new (ptr) T();
    }

    void defaultValue(pointer ptr,
                      bool place) noexcept((is_nothrow_move_assignable<T>::value)
                                           && is_nothrow_move_constructible<T>::value) {
        if (place) {
            placeDefaultTo(ptr);
        } else {
            assignDefaultTo(ptr);
        }
    }

    void assignValueTo(pointer ptr,
                       const_reference value) noexcept(is_nothrow_copy_assignable<T>::value) {
        *ptr = value;
    }

    void placeValueTo(pointer ptr,
                      const_reference value) noexcept(is_nothrow_copy_constructible<T>::value) {
        new (ptr) T(value);
    }

    void copyValue(pointer ptr, const_reference value, bool place) noexcept(
        (is_nothrow_move_assignable<T>::value) && is_nothrow_move_constructible<T>::value) {
        if (place) {
            placeValueTo(ptr, value);
        } else {
            assignValueTo(ptr, value);
        }
    }

    void assignValueTo(pointer ptr,
                       value_type&& value) noexcept(is_nothrow_move_assignable<T>::value) {
        *ptr = std::move(value);
    }

    void placeValueTo(pointer ptr,
                      value_type&& value) noexcept(is_nothrow_move_constructible<T>::value) {
        new (ptr) T(std::move(value));
    }

    void moveValue(pointer ptr,
                   value_type&& value,
                   bool place) noexcept((is_nothrow_move_assignable<T>::value)
                                        && is_nothrow_move_constructible<T>::value) {
        if (place) {
            placeValueTo(ptr, std::move(value));
        } else {
            assignValueTo(ptr, std::move(value));
        }
    }

    void moveOperation(pointer src,
                       pointer dst,
                       size_type num,
                       bool place) noexcept((is_nothrow_move_assignable<T>::value)
                                            && is_nothrow_move_constructible<T>::value) {
        for (size_type i = 0; i < num; ++i) {
            moveValue(dst, std::move(src[i]), place);
            ++dst;
        }
    }

    void destruct(pointer startPos,
                  pointer endPos) noexcept(is_nothrow_destructible<T>::value) {

        while (startPos < endPos) {
            startPos->~T();
            ++startPos;
        }
    }
};


template<class T>
auto TypedVectorBase<T>::erase(iterator first,
                               iterator last) noexcept(is_nothrow_move_assignable<T>::value)
    -> iterator {

    int numToErase = last - first;
    iterator itAfterDeleted = first;

    if (numToErase > 0) {

        size_type numToMove = end() - last;
        ops().moveDown(&*last, &*first, numToMove);

        first += numToMove;
        ops().destruct(first, end());

        proxy.setSize(size() - numToErase);
    }

    return itAfterDeleted;
}


template<class T>
void TypedVectorBase<T>::copyOperation(pointer dst, const_pointer src, size_type num) noexcept(
    is_nothrow_copy_assignable<T>::value&& is_nothrow_copy_constructible<T>::value) {

    pointer dataAlias = data();

    if (dst >= dataAlias) {

        size_type totalNum = static_cast<size_type>(dst - dataAlias) + num;

        for (size_type i = 0; i < num; ++i) {
            ops().copyValue(dst, src[i], (static_cast<size_type>(dst - dataAlias) >= this->size()));
            ++dst;
        }

        ops().destruct(dst, end());
        proxy.setSize(totalNum);
    }
}


template<class T>
template<class CR>
auto TypedVectorBase<T>::insertOperation(
    const_iterator position,
    size_type numToInsert,
    const CR& creatorCall) noexcept(noexcept(creatorCall(nullptr, 1, true))
                                    && is_nothrow_move_assignable<T>::value
                                    && is_nothrow_move_constructible<T>::value) -> iterator {

    if (numToInsert > 0) {

        // First case: insertion target range is subset of the original the b-e range
        //
        //             data             reserved (uninited)
        // |aaaaaaaabbbbbbbbbbbbbbcccc|......................|
        //             to insert
        //          |+++++++++++++|
        // Result:
        // |aaaaaaaa|+++++++++++++|bbb|bbbbbbbbbcccc|........|
        //                 |        |        |
        //                 |        |       moved w placement
        //                 |       moved w assignment
        //                copied w assignment
        //
        // Second case: insertion target range overlaps the original end
        //
        //             data             reserved (uninited)
        // |aaaaaaaaaaaaaaaaaaaabbbbbb|......................|
        //                      |+++++++++++++|
        //                         to insert
        // Result:
        // |aaaaaaaaaaaaaaaaaaaa|+++++|+++++++|bbbbb|........|
        //                         |      |      |
        //                         |      |     moved w placement
        //                         |     copied w placement
        //                        copied w assignment

        auto distanceFromEnd = std::distance(position, cend());
        bool overlapsEnd = (distanceFromEnd < numToInsert);

        size_type movePlacementCnt = overlapsEnd ? distanceFromEnd : numToInsert;
        size_type moveAssignmentCnt = overlapsEnd ? 0 : (distanceFromEnd - numToInsert);
        size_type copyPlacementCnt = overlapsEnd ? (numToInsert - distanceFromEnd) : 0;
        size_type copyAssignmentCnt = movePlacementCnt;

        pointer src = end() - movePlacementCnt;
        pointer dst = end() + numToInsert - movePlacementCnt;

        ops().moveWithPlacement(src, dst, movePlacementCnt);

        src -= moveAssignmentCnt;
        dst -= moveAssignmentCnt;

        ops().moveWithAssignment(src, dst, moveAssignmentCnt);

        ETL_ASSERT((copyAssignmentCnt + copyPlacementCnt) == numToInsert);
#if ETL_VEC_INSERT_OP_DIR_UP
        dst -= (copyAssignmentCnt + copyPlacementCnt);

        if (copyAssignmentCnt > 0) {
            creatorCall(dst, copyAssignmentCnt, false);
            dst += copyAssignmentCnt;
        }

        if (copyPlacementCnt > 0) {
            creatorCall(dst, copyPlacementCnt, true);
            dst += copyPlacementCnt;
        }
#else
        if (copyPlacementCnt > 0) {
            dst -= copyPlacementCnt;
            creatorCall(dst, copyPlacementCnt, true);
        }

        if (copyAssignmentCnt > 0) {
            dst -= copyAssignmentCnt;
            creatorCall(dst, copyAssignmentCnt, false);
        }
#endif
        proxy.setSize(size() + numToInsert);
    }

    return iterator(position);
}


template<class T>
template<class CR>
auto TypedVectorBase<T>::insertOneOperation(
    const_iterator position,
    const CR& creatorCall) noexcept(noexcept(creatorCall(nullptr, true))
                                    && is_nothrow_move_assignable<T>::value
                                    && is_nothrow_move_constructible<T>::value)
    -> iterator {

    size_type distanceFromEnd = std::distance(position, cend());

    auto pos = reinterpret_cast<pointer>(const_cast<iterator>(position));

    if (distanceFromEnd > 0) {
        // When inserting to the middle...
        pointer last = end() - 1U;
        // ... move the last with placement
        ops().placeValueTo(end(), std::move(*last));
        // ... move the others with assignment
        pointer pos = const_cast<pointer>(position);
        ops().moveWithAssignment(pos, (pos + 1U), (distanceFromEnd - 1U));
        // ... assign to the insertion point
        creatorCall(pos, false);
    } else {
        // When inserting to the end
        creatorCall(pos, true);
    }

    proxy.setSize(size() + 1U);

    return iterator(pos);
}


template<class T>
void TypedVectorBase<T>::swapNeighbourRanges(size_type toEndStart, size_type toMidStart) noexcept(
    is_nothrow_move_assignable<T>::value&& is_nothrow_move_constructible<T>::value) {

    if (empty()) {
        return;
    }

    // Swap neighbour ranges on the end of the container.
    //
    //    original data    inserted to end
    // |aaaaaaaaaabbbbbbbbccccccccc|
    //            |       |
    //            |      toMidStart
    //           toEndStart

    ETL_ASSERT(toMidStart >= toEndStart);
    auto cntToEnd = toMidStart - toEndStart;
    ETL_ASSERT(size() > toMidStart);
    auto cntToMid = size() - toMidStart;

    if ((cntToEnd == 0U) || (cntToMid == 0U)) {
        return;
    }

    //   data[toMidStart] contains the first inserted element, to be moved to
    // data[toEndStart]. This is moved to a temporary, then the free slot is filled
    // with the element to be there etc. The end of the algorithm is to have
    // data[toEndStart] as a free slot, to be filled with the temporary.
    // The algorithm shall use cntToMid + cntToEnd steps.
    //
    // Goal      |aaaaaaCCCCCCCCBBB|
    // Given     |aaaaaabbbcccccccc|
    //                  |  |       |
    //        cntToEnd >|--|<      |
    //                  | >|-------|< cntToMid
    //          toEndStart |
    //                    toMidStart
    //
    // S#0       |aaaaaabbb ccScccc|  tmp: c
    //                      __|        |  - make one empty with moving to a temporary,
    //                     |           |    that will be the final position of 'S'
    // S#1       |aaaaaabbbCcc ccSc|   |  - move 'S' to its final position ('C' after move),
    //                                 |    find the next 'S' to insert to the free position ...
    // S#2       |aaaaaabSbCccCcc c|   |  - the next item of I is always
    //                                 |    ((I - toEndStart + cntToEnd) % (cntToEnd +
    //                                 cntToMid)) + toEndStart
    // S#3       |aaaaaab bCScCccBc|   |
    // S#4       |aaaaaabCbC cCScBc|   |
    // S#5       |aaaaaabCbCCcC cBS|   |
    // S#6       |aaaaaabCSCCcCCcB |   |
    // S#7       |aaaaaabC CCSCCcBB|   |
    // S#8       |aaaaaabCCCC CCSBB|   |
    // S#9       |aaaaaaSCCCCCCC BB|   |
    // S#10      |aaaaaa CCCCCCCBBB|   | - ... until the destination of the first element is
    //                  _______________|   reached.
    //                  |
    // S#11      |aaaaaaCCCCCCCCBBB|
    //
    // At this stage the reordering is complete only if
    // cntToEnd and (cntToEnd + cntToMid) are coprimes (?????).
    // In any other case the algorithm finishes with stepCnt < (cntToMid + cntToEnd).
    // E.g. when cntToEnd == 4 and cntToMid == 6
    //    |aabbbbcccccc|
    //    |aabbbb cccSc|  tmp: c
    // 1  |aabbSbCccc c|   |
    // 2  |aabb bCcScBc|   |
    // 3  |aaSbCbCc cBc|   |
    // 4  |aa bCbCcBcBc|   |
    // 5  |aaCbCbCcBcBc|---|
    // In this case you have to repeat the whole sequence
    // with on offset start position:
    //    |aaCbCbC BcBS|  tmp: c
    // 6  |aaCbCSCCBcB |   |
    // 7  |aaCbC CCBSBB|   |
    // 8  |aaCSCCCCB BB|   |
    // 9  |aaC CCCCBBBB|   |
    // 10 |aaCCCCCCBBBB|---|

    auto d = data();

    const size_type totalCnt = cntToMid + cntToEnd;
    auto sourceOf = [toEndStart, cntToEnd, totalCnt](size_type dest) {
        ETL_ASSERT(dest >= toEndStart);
        return ((dest - toEndStart + cntToEnd) % totalCnt) + toEndStart;
    };

    size_type startSlot = toMidStart;
    size_type steps = 0U;
    while (steps < totalCnt) {

        value_type tmp = std::move(d[startSlot]);
        size_type freeSlot = startSlot;
        auto sourceSlot = sourceOf(freeSlot);
        size_type cnt = 0U;
        while (sourceSlot != startSlot) {
            d[freeSlot] = std::move(d[sourceSlot]);
            freeSlot = sourceSlot;
            sourceSlot = sourceOf(freeSlot);
            ++cnt;
            ETL_ASSERT(cnt < totalCnt);
        }

        d[freeSlot] = std::move(tmp);

        ++startSlot;

        ++cnt;
        ETL_ASSERT((totalCnt % cnt) == 0U);
        steps += cnt;
    }
}


template<class T>
void TypedVectorBase<T>::moveFromOther(pointer dst, pointer src, size_type num) noexcept(
    is_nothrow_move_assignable<T>::value&& is_nothrow_move_constructible<T>::value) {

    pointer dataAlias = data();

    ETL_ASSERT(dst >= dataAlias);
    ETL_ASSERT((dst + num) <= (dataAlias + capacity()));

    size_type totalNum = dst - dataAlias + num;

    // Elements are moved in two parts:
    // - the first set is move-assigned to the existing elements if there's any
    size_type cntWithAssignment = 0U;
    if (dst < end()) {
        auto elementsToEnd = static_cast<size_type>(std::distance(dst, end()));
        cntWithAssignment = std::min(elementsToEnd, num);
        ops().moveOperation(src, dst, cntWithAssignment, false);
    }
    // - the second set is move-constructed with placement
    if (cntWithAssignment < num) {
        ops().moveOperation((src + cntWithAssignment),
                            (dst + cntWithAssignment),
                            (num - cntWithAssignment),
                            true);
    }

    if ((dst + num) < end()) {
        ops().destruct(dst + num, end());
    }
    proxy.setSize(totalNum);
}


template<class T>
void TypedVectorBase<T>::clear() noexcept(is_nothrow_destructible<T>::value) {

    ops().destruct(begin(), end());
    proxy.setSize(0);
}


template<class T>
void TypedVectorBase<T>::swapElements(TypedVectorBase<T>& other) noexcept(
    is_nothrow_move_assignable<T>::value&& is_nothrow_move_constructible<T>::value) {

    const auto diff = sizeDiff(*this, other);

    for (size_type i = 0; i < diff.common; ++i) {
        this->swapValues(this->operator[](i), other[i]);
    }

    if (diff.rGreaterWith > 0) {

        this->moveFromOther(getItemPointer(diff.common), &other[diff.common], diff.rGreaterWith);
        other.destruct(&other[diff.common], other.end());
        other.proxy.setSize(diff.common);

    } else if (diff.lGreaterWith > 0) {

        other.moveFromOther(
            other.getItemPointer(diff.common), &this->operator[](diff.common), diff.lGreaterWith);
        this->destruct(&this->operator[](diff.common), this->end());
        this->proxy.setSize(diff.common);
    }
}


#if ETL_USE_EXCEPTIONS

template<typename T>
typename TypedVectorBase<T>::reference TypedVectorBase<T>::at(size_type ix) {

    if (ix >= size()) {
        throw ETL_NAMESPACE::OutOfRangeException();
    }

    return operator[](ix);
}


template<typename T>
typename TypedVectorBase<T>::const_reference TypedVectorBase<T>::at(size_type ix) const {

    if (ix >= size()) {
        throw ETL_NAMESPACE::OutOfRangeException();
    }

    return operator[](ix);
}

#endif

template<typename T>
class TypedVectorBase<T>::DefaultCreator {
  public:
    void operator()(pointer pos, size_type cnt, bool place) const
        noexcept(noexcept(TypedVectorBase::ops().defaultValue(pos, place))) {
        for (size_type i = 0; i < cnt; ++i) {
            TypedVectorBase::ops().defaultValue(pos + i, place);
        }
    }
};


template<typename T>
class TypedVectorBase<T>::CopyCreator {

  private:
    const_reference ref;

  public:
    explicit CopyCreator(const_reference refValue) noexcept :
        ref(refValue) {};
    void operator()(pointer pos, size_type cnt, bool place) const
        noexcept(noexcept(TypedVectorBase::ops().copyValue(pos, ref, place))) {
        for (size_type i = 0; i < cnt; ++i) {
            TypedVectorBase::ops().copyValue(pos + i, ref, place);
        }
    }
};

template<typename T>
template<class InputIt>
class TypedVectorBase<T>::ContCreator {

  private:
    mutable InputIt first;
    mutable InputIt last;

  public:
    ContCreator(InputIt fst,
                InputIt lst) noexcept :
        first(fst),
        last(lst) {};

    size_type getLength() const noexcept {
        return std::distance(first, last);
    }

    void operator()(pointer pos, size_type cnt, bool place) const
        noexcept(noexcept(TypedVectorBase::ops().copyValue(pos, *first, place))) {
#if ETL_VEC_INSERT_OP_DIR_UP
        for (size_t i = 0; i < cnt; ++i) {
            ETL_ASSERT(first != last);
            TypedVectorBase::ops().copyValue(pos + i, *first, place);
            ++first;
        }
#else
        for (int32_t i = (static_cast<int32_t>(cnt) - 1); i >= 0; --i) {
            ETL_ASSERT(first != last);
            --last;
            TypedVectorBase::ops().copyValue(pos + i, *last, place);
        }
#endif
    }
};
}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // __ETL_TYPEDVECTORBASE_H__
