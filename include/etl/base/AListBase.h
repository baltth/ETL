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

#ifndef __ETL_ALISTBASE_H__
#define __ETL_ALISTBASE_H__

#include <etl/base/DoubleChain.h>
#include <etl/base/tools.h>
#include <etl/etlSupport.h>

#include <utility>

namespace ETL_NAMESPACE {
namespace Detail {


class AListBase {

  public:  // types

    class Node : public DoubleChain::Node {

      protected:

        Node() noexcept = default;

        explicit Node(const DoubleChain::Node& other) noexcept :
            DoubleChain::Node(other) {};
    };

    class Iterator {
        friend class AListBase;

      protected:

        AListBase::Node* node;

      public:

        Iterator() = delete;
        Iterator(const Iterator& other) noexcept = default;
        Iterator& operator=(const Iterator& other) & noexcept = default;
        Iterator(Iterator&& other) noexcept = default;
        Iterator& operator=(Iterator&& other) & noexcept = default;
        ~Iterator() noexcept = default;

        bool operator==(const Iterator& other) const noexcept {
            return (node == other.node);
        }

        bool operator!=(const Iterator& other) const noexcept {
            return !(operator==(other));
        }

        Iterator& operator++() noexcept {
            node = static_cast<AListBase::Node*>(node->next);
            return *this;
        }

        Iterator& operator--() noexcept {
            node = static_cast<AListBase::Node*>(node->prev);
            return *this;
        }

      protected:

        explicit Iterator(AListBase::Node* n) noexcept :
            node(n) {};
    };

    typedef uint32_t size_type;

  protected:  // variables

    DoubleChain chain;
    size_type size_ = 0U;

  public:  // functions

    AListBase() noexcept = default;

    AListBase(const AListBase& other) = delete;
    AListBase& operator=(const AListBase& other) = delete;

    AListBase(AListBase&& other) noexcept = default;
    AListBase& operator=(AListBase&& other) noexcept = default;
    ~AListBase() noexcept = default;

    size_type size() const noexcept {
        return size_;
    }

    bool empty() const noexcept {
        return (size_ == 0U);
    }

  protected:

    Iterator begin() const noexcept {
        return Iterator(static_cast<AListBase::Node*>(chain.getFirst()));
    }

    Iterator end() const noexcept {
        return Iterator(static_cast<AListBase::Node*>(chain.getLast()->next));
    }

    /// \name Element operations
    /// \{
    void pushFront(Node& item) noexcept {
        chain.insertBefore(chain.getFirst(), &item);
        ++size_;
    }

    void pushBack(Node& item) noexcept {
        chain.insertAfter(chain.getLast(), &item);
        ++size_;
    }

    Node* popFront() noexcept;
    Node* popBack() noexcept;

    void insert(Iterator pos, Node& item) noexcept {
        chain.insertBefore(pos.node, &item);
        ++size_;
    }

    Node* remove(Iterator pos) noexcept {
        --size_;
        return static_cast<AListBase::Node*>(chain.remove(pos.node));
    }

    void replace(Node& n1, Node& n2) noexcept {
        chain.replace(&n1, &n2);
    }

    void swapNodeList(AListBase& other) noexcept;
    void splice(Iterator pos,
                AListBase& other,
                Iterator first,
                Iterator last) noexcept;
    /// \}
};

static_assert(NothrowContract<AListBase::Node>::value,
              "AListBase::Node violates nothrow contract");
static_assert(NothrowContract<AListBase::Iterator>::value,
              "AListBase::Iterator violates nothrow contract");
static_assert(NothrowContract<AListBase>::value,
              "AListBase violates nothrow contract");

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // __ETL_ALISTBASE_H__
