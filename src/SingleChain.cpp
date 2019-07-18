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

#include <etl/base/SingleChain.h>

using ETL_NAMESPACE::Detail::SingleChain;


SingleChain::SingleChain(SingleChain&& other) {

    setEmpty();
    swap(other);
}


SingleChain& SingleChain::operator=(SingleChain&& other) {

    swap(other);
    return *this;
}


void SingleChain::insertAfter(Node* pos, Node* node) {

    ETL_ASSERT(pos != nullptr);
    ETL_ASSERT(node != nullptr);

    Node* next = pos->next;
    linkNodes(pos, node);
    linkNodes(node, next);
}


SingleChain::Node* SingleChain::removeAfter(Node* pos) {

    ETL_ASSERT(pos != nullptr);
    ETL_ASSERT(pos->next != nullptr);

    Node* removed = pos->next;
    linkNodes(pos, pos->next->next);
    removed->next = nullptr;

    return removed;
}


void SingleChain::takeListOf(SingleChain& other) {

    if (&other != this) {
        linkNodes(&frontNode, other.getFirst());
        other.setEmpty();
    }
}


void SingleChain::swap(SingleChain& other) {

    if (&other != this) {

        if ((!isEmpty()) && (!other.isEmpty())) {

            SingleChain tmp;
            tmp.takeListOf(*this);
            takeListOf(other);
            other.takeListOf(tmp);

        } else if (!isEmpty()) {

            other.takeListOf(*this);

        } else if (!other.isEmpty()) {

            takeListOf(other);
        }
    }
}
