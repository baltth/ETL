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

#include <etl/etlSupport.h>
#include <etl/base/DoubleChain.h>

#include <utility>

namespace ETL_NAMESPACE {


class AListBase {

  public:   // types

    class Node : public DoubleChain::Node {

      protected:

        Node() :
            DoubleChain::Node() {};

        explicit Node(const DoubleChain::Node& other) :
            DoubleChain::Node(other) {};

    };

    class Iterator {
        friend class AListBase;

      protected:

        AListBase::Node* node;

      public:

        bool operator==(const Iterator& other) const {
            return (node == other.node);
        }

        bool operator!=(const Iterator& other) const {
            return !(operator==(other));
        }

        Iterator& operator++() {
            node = static_cast<AListBase::Node*>(node->next);
            return *this;
        }

        Iterator& operator--() {
            node = static_cast<AListBase::Node*>(node->prev);
            return *this;
        }

      protected:

        explicit Iterator(AListBase::Node* n) :
            node(n) {};

    };

  protected: // variables

    DoubleChain list;
    uint32_t size_;

  public:   // functions

    AListBase() :
        size_(0U) {};

#if ETL_USE_CPP11

    AListBase(AListBase&& other) :
        list(std::move(other.list)),
        size_(other.size_) {};

    AListBase& operator=(AListBase&& other) {
        list = std::move(other.list);
        size_ = other.size_;
        return *this;
    }

#endif

    uint32_t size() const {
        return size_;
    }

    bool empty() const {
        return (size_ == 0U);
    }

  protected:

    Iterator begin() const {
        return Iterator(static_cast<AListBase::Node*>(list.getFirst()));
    }

    Iterator end() const {
        return Iterator(static_cast<AListBase::Node*>(list.getLast()->next));
    }

    /// \name Element operations
    /// \{
    void pushFront(Node* item) {
        if (item != NULLPTR) {
            list.insertBefore(list.getFirst(), item);
            ++size_;
        }
    }

    void pushBack(Node* item) {
        if (item != NULLPTR) {
            list.insertAfter(list.getLast(), item);
            ++size_;
        }
    }

    Node* popFront();
    Node* popBack();

    void insert(Iterator pos, Node* item) {
        if (item != NULLPTR) {
            list.insertBefore(pos.node, item);
            ++size_;
        }
    }

    Node* remove(Iterator pos) {
        --size_;
        return static_cast<AListBase::Node*>(list.remove(pos.node));
    }

    void replace(Node* n1, Node* n2) {
        list.replace(n1, n2);
    }

    void swapNodeList(AListBase& other);
    void splice(Iterator pos,
                AListBase& other,
                Iterator first,
                Iterator last);
    /// \}

  private:

    AListBase(const AListBase& other);
    AListBase& operator=(const AListBase& other);

};

}

#endif /* __ETL_ALISTBASE_H__ */

