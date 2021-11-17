/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2019-2021 Balazs Toth.

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

#ifndef __ETL_SPAN_H__
#define __ETL_SPAN_H__

#include <etl/base/tools.h>
#include <etl/etlSupport.h>
#include <etl/traitSupport.h>

#include <array>
#include <cstddef>
#include <iterator>

namespace ETL_NAMESPACE {

namespace Detail {

template<class From, class To>
struct EnableIfArrayDataConvertible :
    enable_if<std::is_convertible<From (*)[], To (*)[]>::value> {};

template<class From, class To>
using EnableIfArrayDataConvertibleType = typename EnableIfArrayDataConvertible<From, To>::type;

}  // namespace Detail


static const std::size_t dynamic_extent = -1;


template<class T, std::size_t Extent = dynamic_extent>
class Span {

  public:  // types

    using element_type = T;
    using value_type = remove_cv_t<T>;
    using index_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static constexpr std::size_t extent = Extent;

  private:  // variables

    pointer data_;
    std::size_t size_;

  public:  // functions

    /// \name Construction, destruction, assignment
    /// \{

    template<bool Dep = false,
             class = enable_if_t<Dep || (extent == 0U) || (extent == dynamic_extent)>>
    constexpr Span() noexcept :
        data_(nullptr),
        size_(0U) {}

    constexpr Span(pointer ptr, index_type count) :
        data_(ptr),
        size_(count) {}

    constexpr Span(pointer first, pointer last) :
        Span(first, (last - first)) {}

    template<std::size_t N,
             class = enable_if_t<(extent == N) || (extent == dynamic_extent)>>
    constexpr Span(element_type (&arr)[N]) noexcept :
        data_(arr),
        size_(N) {}

    template<std::size_t N,
             class = enable_if_t<(extent == N) || (extent == dynamic_extent)>,
             class From = value_type,
             class = Detail::EnableIfArrayDataConvertibleType<From, element_type>>
    constexpr Span(std::array<value_type, N>& arr) noexcept :
        data_(&std::get<0>(arr)),
        size_(N) {}

    template<std::size_t N,
             class = enable_if_t<(extent == N) || (extent == dynamic_extent)>,
             class From = add_const_t<value_type>,
             class = Detail::EnableIfArrayDataConvertibleType<From, element_type>>
    constexpr Span(const std::array<value_type, N>& arr) noexcept :
        data_(&std::get<0>(arr)),
        size_(N) {}

    template<
        class Cont,
        class = Detail::EnableIfArrayDataConvertibleType<typename Cont::value_type, element_type>>
    constexpr Span(Cont& container) :
        data_(container.data()),
        size_(container.size()) {}

    template<
        class Cont,
        class = Detail::EnableIfArrayDataConvertibleType<add_const_t<typename Cont::value_type>,
                                                         element_type>>
    constexpr Span(const Cont& container) :
        data_(container.data()),
        size_(container.size()) {}

    template<class U,
             std::size_t N,
             class = enable_if_t<(extent == N) || (extent == dynamic_extent)>,
             class = Detail::EnableIfArrayDataConvertibleType<U, element_type>>
    constexpr Span(const Span<U, N>& other) noexcept :
        data_(other.data()),
        size_(other.size()) {}

    constexpr Span(const Span& other) noexcept = default;
    Span& operator=(const Span& other) noexcept = default;
    /// \}

    /// \name Capacity
    /// \{

    constexpr index_type size() const noexcept {
        return size_;
    }

    constexpr index_type size_bytes() const noexcept {
        return size() * sizeof(element_type);
    }

    constexpr bool empty() const noexcept {
        return (size_ == 0U);
    }
    /// \}

    /// \name Iterators
    /// \{

    ETL_COND_CONSTEXPR iterator begin() const noexcept {
        return data();
    }

    ETL_COND_CONSTEXPR const_iterator cbegin() const noexcept {
        return data();
    }

    ETL_COND_CONSTEXPR iterator end() const noexcept {
        return data() + size();
    }

    ETL_COND_CONSTEXPR const_iterator cend() const noexcept {
        return data() + size();
    }

    ETL_COND_CONSTEXPR reverse_iterator rbegin() const noexcept {
        return reverse_iterator(this->end());
    }

    ETL_COND_CONSTEXPR const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(this->cend());
    }

    ETL_COND_CONSTEXPR reverse_iterator rend() const noexcept {
        return reverse_iterator(this->begin());
    }

    ETL_COND_CONSTEXPR const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(this->cbegin());
    }
    /// \}

    /// \name Element access
    /// \{

    ETL_COND_CONSTEXPR reference front() const {
        return *(begin());
    }

    ETL_COND_CONSTEXPR reference back() const {
        return data_[size() - 1];
    }

    ETL_COND_CONSTEXPR reference operator[](index_type ix) const {
        return data_[ix];
    }

    ETL_COND_CONSTEXPR pointer data() const noexcept {
        return data_;
    }
    /// \}

    /// \name Subviews
    /// \{

    template<std::size_t Cnt>
    constexpr Span<element_type, Cnt> first() const {
        static_assert((extent == dynamic_extent) || (Cnt <= extent), "Invalid first<count>()");
        return Span<element_type, Cnt>(data_, Cnt);
    }

    constexpr Span<element_type, dynamic_extent> first(std::size_t cnt) const {
        return Span<element_type, dynamic_extent>(data_, cnt);
    }

    template<std::size_t Cnt>
    constexpr Span<element_type, Cnt> last() const {
        static_assert((extent == dynamic_extent) || (Cnt <= extent), "Invalid last<count>()");
        return Span<element_type, Cnt>(data_ + size() - Cnt, Cnt);
    }

    constexpr Span<element_type, dynamic_extent> last(std::size_t cnt) const {
        return Span<element_type, dynamic_extent>(data_ + size() - cnt, cnt);
    }

    template<std::size_t Offs,
             std::size_t Cnt = dynamic_extent,
             class = enable_if_t<Cnt != dynamic_extent>>
    constexpr Span<element_type, Cnt> subspan() const {
        static_assert((extent == dynamic_extent) || ((Offs + Cnt) <= extent),
                      "Invalid subspan<offs, count>()");
        return Span<element_type, dynamic_extent>(data_ + Offs, Cnt);
    }

    template<std::size_t Offs,
             std::size_t Cnt = dynamic_extent,
             class = enable_if_t<(Cnt == dynamic_extent) && (extent != dynamic_extent)>>
    constexpr Span<element_type, (extent - Offs)> subspan() const {
        static_assert(Offs <= extent, "Invalid subspan<offs, count>()");
        return Span<element_type, (extent - Offs)>(data_ + Offs, size() - Offs);
    }

    template<std::size_t Offs,
             std::size_t Cnt = dynamic_extent,
             class = enable_if_t<(Cnt == dynamic_extent) && (extent == dynamic_extent)>>
    ETL_COND_CONSTEXPR Span<element_type, dynamic_extent> subspan() const {
        return Span<element_type, dynamic_extent>(data_ + Offs, size() - Offs);
    }

    ETL_COND_CONSTEXPR Span<element_type, dynamic_extent>
    subspan(std::size_t offs,
            std::size_t cnt = dynamic_extent) const {
        return Span<element_type, dynamic_extent>(data_ + offs,
                                                  (cnt == dynamic_extent) ? (size() - offs) : cnt);
    }
    /// \}
};


template<class T, std::size_t Extent>
constexpr std::size_t Span<T, Extent>::extent;

}  // namespace ETL_NAMESPACE

#endif  // __ETL_SPAN_H__
