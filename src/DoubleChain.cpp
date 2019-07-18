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

#include <etl/base/DoubleChain.h>

using ETL_NAMESPACE::Detail::DoubleChain;


DoubleChain::DoubleChain(DoubleChain&& other) noexcept {

    setEmpty();
    swap(other);
}


DoubleChain& DoubleChain::operator=(DoubleChain&& other) noexcept {

    swap(other);
    return *this;
}


void DoubleChain::insertAfter(Node* pos, Node* node) noexcept {

    ETL_ASSERT(pos != nullptr);
    ETL_ASSERT(pos != &backNode);
    ETL_ASSERT(node != nullptr);

    Node* next = pos->next;
    linkNodes(pos, node);
    linkNodes(node, next);
}


void DoubleChain::insertBefore(Node* pos, Node* node) noexcept {

    ETL_ASSERT(pos != nullptr);
    ETL_ASSERT(pos != &frontNode);
    ETL_ASSERT(node != nullptr);

    Node* prev = pos->prev;
    linkNodes(node, pos);
    linkNodes(prev, node);
}


DoubleChain::Node* DoubleChain::remove(Node* node) noexcept {

    ETL_ASSERT(node != nullptr);
    ETL_ASSERT(node != &backNode);
    ETL_ASSERT(node != &frontNode);

    linkNodes(node->prev, node->next);
    node->prev = nullptr;
    node->next = nullptr;

    return node;
}


void DoubleChain::replace(Node* n1, Node* n2) noexcept {

    ETL_ASSERT(n1 != nullptr);
    ETL_ASSERT(n2 != nullptr);

    if ((n1 != n2)) {

        Node tmp = *n2;

        linkNodesProtected(n1->prev, n2);
        linkNodesProtected(n2, n1->next);

        linkNodesProtected(tmp.prev, n1);
        linkNodesProtected(n1, tmp.next);
    }
}


void DoubleChain::setEmpty() noexcept {

    frontNode.next = &backNode;
    backNode.prev = &frontNode;
}


void DoubleChain::takeListOf(DoubleChain& other) noexcept {

    if (&other != this) {
        linkNodes(&frontNode, other.getFirst());
        linkNodes(other.getLast(), &backNode);
        other.setEmpty();
    }
}


void DoubleChain::swap(DoubleChain& other) noexcept {

    if (&other != this) {

        if ((!isEmpty()) && (!other.isEmpty())) {

            DoubleChain tmp;
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
