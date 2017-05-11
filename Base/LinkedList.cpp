/**
\file
\date 2016.01.19.
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

#include "Base/LinkedList.h"

using namespace ETL_NAMESPACE;

#if ETL_USE_CPP11

LinkedList::LinkedList(LinkedList&& other) {

    setEmpty();
    swap(other);
}


LinkedList& LinkedList::operator=(LinkedList&& other) {

    swap(other);
    return *this;
}

#endif

void LinkedList::insertAfter(Node* pos, Node* node) {

    if (pos != NULLPTR) {
        Node* next = pos->next;
        linkNodes(pos, node);
        linkNodes(node, next);
        ++size;
    }
}


LinkedList::Node* LinkedList::removeAfter(Node* pos) {

    Node* removed = NULLPTR;

    if (size > 0) {

        if ((pos != NULLPTR) && (pos->next != NULLPTR)) {
            removed = pos->next;
            removed->next = NULLPTR;
            linkNodes(pos, pos->next->next);
            --size;
        }
    }

    return removed;
}


void LinkedList::copy(const LinkedList& other) {

    frontNode.next = other.getFirst();
    size = other.getSize();
}


void LinkedList::swap(LinkedList& other) {

    if ((getSize() > 0) && (other.getSize() > 0)) {

        Node* tmpFirst = getFirst();
        uint32_t tmpSize = getSize();

        copy(other);

        other.frontNode.next = tmpFirst;
        other.size = tmpSize;

    } else if (getSize() > 0) {

        other.copy(*this);
        setEmpty();

    } else if (other.getSize() > 0) {

        copy(other);
        other.setEmpty();
    }
}


void LinkedList::setEmpty() {

    frontNode.next = NULLPTR;
    size = 0;
}


void LinkedList::linkNodes(Node* a, Node* b) {

    a->next = b;
}

