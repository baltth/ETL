﻿/**
\file
\date 2016.01.19. 11:43:18
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

#include "DoubleLinkedList.h"

using namespace ETL_NAMESPACE;


DoubleLinkedList::DoubleLinkedList(DoubleLinkedList &&other) {

    setEmpty();
    swap(other);
}


DoubleLinkedList &DoubleLinkedList::operator=(DoubleLinkedList &&other) {

    swap(other);
    return *this;
}


void DoubleLinkedList::insertAfter(Node* pos, Node* node) {

    if(pos != &backNode) {
        Node* next = pos->next;
        linkNodes(pos, node);
        linkNodes(node, next);
        ++size;
    }
}


void DoubleLinkedList::insertBefore(Node* pos, Node* node) {

    if(pos != &frontNode) {
        Node* prev = pos->prev;
        linkNodes(node, pos);
        linkNodes(prev, node);
        ++size;
    }
}


DoubleLinkedList::Node* DoubleLinkedList::remove(Node* node) {

    if(size > 0) {

        if((node != &frontNode) && (node != &backNode)) {
            linkNodes(node->prev, node->next);
        }

        --size;

        node->prev = nullptr;
        node->next = nullptr;
    }

    return node;
}


void DoubleLinkedList::copy(const DoubleLinkedList &other) {

    frontNode.next = other.getFirst();
    backNode.prev = other.getLast();
    size = other.getSize();
}


void DoubleLinkedList::swap(DoubleLinkedList &other) {

    if((getSize() > 0) && (other.getSize() > 0)) {

        Node* tmpFirst = getFirst();
        Node* tmpLast = getLast();
        uint32_t tmpSize = getSize();

        copy(other);

        other.frontNode.next = tmpFirst;
        other.backNode.prev = tmpLast;
        other.size = tmpSize;

    } else if(getSize() > 0) {

        other.copy(*this);
        setEmpty();

    } else if(other.getSize() > 0) {

        copy(other);
        other.setEmpty();
    }
}


void DoubleLinkedList::setEmpty() {

    frontNode.next = &backNode;
    backNode.prev = &frontNode;
    size = 0;
}


void DoubleLinkedList::linkNodes(Node* a, Node* b) {

    a->next = b;
    b->prev = a;
}
