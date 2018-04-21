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

#include <ETL/base/DoubleLinkedList.h>

using ETL_NAMESPACE::DoubleLinkedList;


#if ETL_USE_CPP11

DoubleLinkedList::DoubleLinkedList(DoubleLinkedList&& other) {

    setEmpty();
    swap(other);
}


DoubleLinkedList& DoubleLinkedList::operator=(DoubleLinkedList&& other) {

    swap(other);
    return *this;
}

#endif

void DoubleLinkedList::insertAfter(Node* pos, Node* node) {

    if ((pos) && (node)) {
        if (pos != &backNode) {
            Node* next = pos->next;
            linkNodes(pos, node);
            linkNodes(node, next);
            ++size;
        }
    }
}


void DoubleLinkedList::insertBefore(Node* pos, Node* node) {

    if ((pos) && (node)) {
        if (pos != &frontNode) {
            Node* prev = pos->prev;
            linkNodes(node, pos);
            linkNodes(prev, node);
            ++size;
        }
    }
}


DoubleLinkedList::Node* DoubleLinkedList::remove(Node* node) {

    if ((size > 0) && (node)) {
        if ((node != &frontNode) && (node != &backNode)) {

            linkNodes(node->prev, node->next);
            --size;
            node->prev = NULLPTR;
            node->next = NULLPTR;
        }
    }

    return node;
}


void DoubleLinkedList::getListOf(DoubleLinkedList& other) {

    if (&other != this) {
        linkNodes(&frontNode, other.getFirst());
        linkNodes(other.getLast(), &backNode);
        size = other.getSize();
        other.setEmpty();
    }
}


void DoubleLinkedList::swap(DoubleLinkedList& other) {

    if (&other != this) {

        if ((getSize() > 0) && (other.getSize() > 0)) {

            DoubleLinkedList tmp;
            tmp.getListOf(*this);
            getListOf(other);
            other.getListOf(tmp);

        } else if (getSize() > 0) {

            other.getListOf(*this);

        } else if (other.getSize() > 0) {

            getListOf(other);
        }
    }
}


void DoubleLinkedList::setEmpty() {

    frontNode.next = &backNode;
    backNode.prev = &frontNode;
    size = 0;
}

