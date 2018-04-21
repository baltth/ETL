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

#include <ETL/base/LinkedList.h>

using ETL_NAMESPACE::LinkedList;


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


void LinkedList::getListOf(LinkedList& other) {

    if (&other != this) {
        linkNodes(&frontNode, other.getFirst());
        size = other.getSize();
        other.setEmpty();
    }
}


void LinkedList::swap(LinkedList& other) {

    if (&other != this) {

        if ((getSize() > 0) && (other.getSize() > 0)) {

            LinkedList tmp;
            tmp.getListOf(*this);

            getListOf(other);
            other.getListOf(tmp);

        } else if (getSize() > 0) {

            other.getListOf(*this);
            setEmpty();

        } else if (other.getSize() > 0) {

            getListOf(other);
            other.setEmpty();
        }
    }
}


void LinkedList::setEmpty() {

    frontNode.next = NULLPTR;
    size = 0;
}

