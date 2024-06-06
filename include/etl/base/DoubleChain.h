/** \file
\author Balazs Toth - baltth@gmail.com

\copyright
\parblock
Copyright 2016-2024 Balazs Toth.

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

#ifndef ETL_DOUBLECHAIN_H_
#define ETL_DOUBLECHAIN_H_

#include <etl/base/tools.h>
#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {
namespace Detail {


class DoubleChain {

  public:  // types

    class Node {

      public:

        Node* prev;
        Node* next;

        Node() noexcept :
            prev(nullptr),
            next(nullptr) {};

        Node(Node* p, Node* n) noexcept :
            prev(p),
            next(n) {};
    };

  private:  // variables

    Node frontNode;
    Node backNode;

  public:  // functions

    DoubleChain() noexcept :
        frontNode(nullptr, &backNode),
        backNode(&frontNode, nullptr) {};

    DoubleChain(const DoubleChain& other) = delete;
    DoubleChain& operator=(const DoubleChain& other) = delete;

    DoubleChain(DoubleChain&& other) noexcept :
        DoubleChain() {
        takeListOf(other);
        other.setEmpty();
    }

    DoubleChain& operator=(DoubleChain&& other) noexcept {
        swap(other);
        return *this;
    }

    ~DoubleChain() noexcept = default;

    bool isEmpty() const noexcept {
        return (frontNode.next == &backNode);
    }

    Node* getFirst() const noexcept {
        return frontNode.next;
    }

    Node* getLast() const noexcept {
        return backNode.prev;
    }

    void insertAfter(Node* pos, Node* node) noexcept;
    void insertBefore(Node* pos, Node* node) noexcept;

    Node* remove(Node* node) noexcept;
    void replace(Node* n1, Node* n2) noexcept;
    void setEmpty() noexcept;

    void swap(DoubleChain& other) noexcept;

  private:

    void takeListOf(DoubleChain& other) noexcept;

    static void linkNodes(Node* a, Node* b) noexcept {
        a->next = b;
        b->prev = a;
    }

    static void linkNodesProtected(Node* a, Node* b) noexcept {
        if (a != nullptr) {
            a->next = b;
        }
        if (b != nullptr) {
            b->prev = a;
        }
    }
};

static_assert(NothrowContract<DoubleChain::Node>::value,
              "DoubleChain::Node violates nothrow contract");
static_assert(NothrowContract<DoubleChain>::value,
              "DoubleChain violates nothrow contract");

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // ETL_DOUBLECHAIN_H_
