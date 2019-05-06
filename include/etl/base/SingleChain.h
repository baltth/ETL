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

#ifndef __ETL_SINGLECHAIN_H__
#define __ETL_SINGLECHAIN_H__

#include <etl/etlSupport.h>

namespace ETL_NAMESPACE {
namespace Detail {


class SingleChain {

  public:  // types

    class Node {

      public:

        Node* next;

        Node() :
            next(nullptr) {};

        explicit Node(Node* n) :
            next(n) {};
    };

  private:  // variables

    Node frontNode;

  public:  // functions

    SingleChain() = default;

    SingleChain(const SingleChain& other) = delete;
    SingleChain& operator=(const SingleChain& other) = delete;

    SingleChain(SingleChain&& other) noexcept {
        takeListOf(other);
        other.setEmpty();
    }

    SingleChain& operator=(SingleChain&& other) noexcept {
        swap(other);
        return *this;
    }

    ~SingleChain() = default;

    bool isEmpty() const noexcept {
        return (frontNode.next == nullptr);
    }

    Node* getFirst() const noexcept {
        return frontNode.next;
    }

    Node& getFrontNode() noexcept {
        return frontNode;
    }

    const Node& getFrontNode() const noexcept {
        return frontNode;
    }

    void insertAfter(Node* pos, Node* node) noexcept;
    Node* removeAfter(Node* pos) noexcept;

    void setEmpty() noexcept {
        frontNode.next = nullptr;
    }

    void swap(SingleChain& other) noexcept;

  private:

    void takeListOf(SingleChain& other) noexcept;

    static void linkNodes(Node* a, Node* b) noexcept {
        a->next = b;
    }
};

}  // namespace Detail
}  // namespace ETL_NAMESPACE

#endif  // __ETL_SINGLECHAIN_H__
