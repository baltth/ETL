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

#ifndef __ETL_DOUBLELINKEDLIST_H__
#define __ETL_DOUBLELINKEDLIST_H__

#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {


class DoubleLinkedList {

  public:   // types

    class Node {

      public:

        Node* prev;
        Node* next;

        Node() :
            prev(NULLPTR),
            next(NULLPTR) {};

        Node(Node* p, Node* n) :
            prev(p),
            next(n) {};

    };

  private:  // variables

    Node frontNode;
    Node backNode;
    uint32_t size_;

  public:   // functions

    DoubleLinkedList() :
        frontNode(NULLPTR, &backNode),
        backNode(&frontNode, NULLPTR),
        size_(0) {};

#if ETL_USE_CPP11

    DoubleLinkedList(DoubleLinkedList&& other);
    DoubleLinkedList& operator=(DoubleLinkedList&& other);

#endif

    bool empty() const {
        return size_ == 0;
    }

    uint32_t size() const {
        return size_;
    }

    Node* getFirst() const {
        return frontNode.next;
    }

    Node* getLast() const {
        return backNode.prev;
    }

    void insertAfter(Node* pos, Node* node);
    void insertBefore(Node* pos, Node* node);

    Node* remove(Node* node);
    void replace(Node* n1, Node* n2);
    void setEmpty();

    void swap(DoubleLinkedList& other);

  private:

    DoubleLinkedList(const DoubleLinkedList& other);
    DoubleLinkedList& operator=(const DoubleLinkedList& other);

    void getListOf(DoubleLinkedList& other);

    static void linkNodes(Node* a, Node* b) {
        a->next = b;
        b->prev = a;
    }

    static void linkNodesProtected(Node* a, Node* b) {
        if (a != NULLPTR) {
            a->next = b;
        }
        if (b != NULLPTR) {
            b->prev = a;
        }
    }
};

}

#endif /* __ETL_DOUBLELINKEDLIST_H__ */

