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
#include <etl/base/DoubleLinkedList.h>

#include <utility>

namespace ETL_NAMESPACE {


class AListBase {

  public:   // types

    class Node : public DoubleLinkedList::Node {

      protected:

        Node() :
            DoubleLinkedList::Node() {};

        explicit Node(const DoubleLinkedList::Node& other) :
            DoubleLinkedList::Node(other) {};

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

    DoubleLinkedList list;

  public:   // functions

    AListBase() {};

#if ETL_USE_CPP11

    AListBase(AListBase&& other) :
        list(std::move(other.list)) {};

    AListBase& operator=(AListBase&& other) {
        list = std::move(other.list);
        return *this;
    }

#endif

    uint32_t getSize() const {
        return list.getSize();
    }

    bool isEmpty() const {
        return list.isEmpty();
    }

  protected:

    Iterator begin() const {
        return Iterator(static_cast<AListBase::Node*>(list.getFirst()));
    }

    Iterator end() const {
        return Iterator(static_cast<AListBase::Node*>(list.getLast()->next));
    }

    /// \name Element operations
    /// @{
    void pushFront(Node* item) {
        list.insertBefore(list.getFirst(), item);
    }

    void pushBack(Node* item) {
        list.insertAfter(list.getLast(), item);
    }

    Node* popFront();
    Node* popBack();

    void insert(Iterator pos, Node* item) {
        list.insertBefore(pos.node, item);
    }

    Node* remove(Iterator pos) {
        return static_cast<AListBase::Node*>(list.remove(pos.node));
    }

    void replace(Iterator it1, Iterator it2) {
        list.replace(it1.node, it2.node);
    }

    void replace(Node& n1, Node& n2) {
        list.replace(&n1, &n2);
    }

    void swapNodeList(AListBase& other) {
        list.swap(other.list);
    }

    void splice(Iterator pos,
                AListBase& other,
                Iterator first,
                Iterator last);
    /// @}

  private:

    AListBase(const AListBase& other);
    AListBase& operator=(const AListBase& other);

};

}

#endif /* __ETL_ALISTBASE_H__ */

