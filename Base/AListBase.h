/**
\file
\date 2015. 05. 21. 20:49:44
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

#ifndef __ETL_ALISTBASE_H__
#define __ETL_ALISTBASE_H__

#include "etlSupport.h"

#include <utility>

#include "Base/DoubleLinkedList.h"


namespace ETL_NAMESPACE {


class AListBase {

// types
public:

    class Node : public DoubleLinkedList::Node {

    public:

        Node() :
            DoubleLinkedList::Node() {};

        explicit Node(const DoubleLinkedList::Node &other) :
            DoubleLinkedList::Node(other) {};

        virtual ~Node() {};

    };

    class Iterator {
        friend class AListBase;

    protected:

        AListBase::Node* node;

    public:

        bool operator==(const Iterator &other) const {
            return (node == other.node);
        }

        bool operator!=(const Iterator &other) const {
            return !(operator==(other));
        }

        Iterator &operator++() {
            node = static_cast<AListBase::Node*>(node->next);
            return *this;
        }

        Iterator &operator--() {
            node = static_cast<AListBase::Node*>(node->prev);
            return *this;
        }

    protected:

        explicit Iterator(AListBase::Node* n) :
            node(n) {};

    };

// variables
protected:

    DoubleLinkedList list;

// functions
public:

    AListBase() {};

#if ETL_USE_CPP11

    AListBase(AListBase &&other) :
        list(std::move(other.list)) {};

    AListBase &operator=(AListBase &&other) {
        list = std::move(other.list);
        return *this;
    }

#endif

    virtual ~AListBase() {}

    inline uint32_t getSize() const {
        return list.getSize();
    }

protected:

    inline Iterator begin() const {
        return Iterator(static_cast<AListBase::Node*>(list.getFirst()));
    }

    inline Iterator end() const {
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
    /// @}

private:

    AListBase(const AListBase &other);
    AListBase &operator=(const AListBase &other);

};

}

#endif /* __ETL_ALISTBASE_H__ */

