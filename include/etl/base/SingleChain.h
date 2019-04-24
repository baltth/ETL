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

  public:   // types

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

  public:   // functions

    SingleChain() :
        frontNode() {};

    SingleChain(SingleChain&& other);
    SingleChain& operator=(SingleChain&& other);

    bool isEmpty() const {
        return (frontNode.next == nullptr);
    }

    Node* getFirst() const {
        return frontNode.next;
    }

    Node& getFrontNode() {
        return frontNode;
    }

    const Node& getFrontNode() const {
        return frontNode;
    }

    void insertAfter(Node* pos, Node* node);
    Node* removeAfter(Node* pos);

    void setEmpty() {
        frontNode.next = nullptr;
    }

    void swap(SingleChain& other);

  private:

    SingleChain(const SingleChain& other);
    SingleChain& operator=(const SingleChain& other);

    void takeListOf(SingleChain& other);

    static void linkNodes(Node* a, Node* b) {
        a->next = b;
    }

};

}
}

#endif /* __ETL_SINGLECHAIN_H__ */

