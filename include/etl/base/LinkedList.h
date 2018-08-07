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

#ifndef __ETL_LINKEDLIST_H__
#define __ETL_LINKEDLIST_H__

#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {


class LinkedList {

  public:   // types

    class Node {

      public:

        Node* next;

        Node() :
            next(NULLPTR) {};

        explicit Node(Node* n) :
            next(n) {};

    };

  private:  // variables

    Node frontNode;
    uint32_t size_;

  public:   // functions

    LinkedList() :
        frontNode(),
        size_(0) {};

#if ETL_USE_CPP11

    LinkedList(LinkedList&& other);
    LinkedList& operator=(LinkedList&& other);

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

    void insertAfter(Node* pos, Node* node);
    Node* removeAfter(Node* pos);
    void setEmpty();

    void swap(LinkedList& other);

  private:

    LinkedList(const LinkedList& other);
    LinkedList& operator=(const LinkedList& other);

    void getListOf(LinkedList& other);

    static void linkNodes(Node* a, Node* b) {
        a->next = b;
    }

};

}

#endif /* __ETL_LINKEDLIST_H__ */

