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

#include <etl/base/AListBase.h>

using ETL_NAMESPACE::AListBase;


AListBase::Node* AListBase::popFront() {

    if (size_ > 0) {
        --size_;
        return static_cast<Node*>(list.remove(list.getFirst()));
    } else {
        return NULLPTR;
    }
}

AListBase::Node* AListBase::popBack() {

    if (size_ > 0) {
        --size_;
        return static_cast<Node*>(list.remove(list.getLast()));
    } else {
        return NULLPTR;
    }
}


void AListBase::swapNodeList(AListBase& other) {

    list.swap(other.list);

    uint32_t tmpSize = size_;
    size_ = other.size_;
    other.size_ = tmpSize;
}


void AListBase::splice(Iterator pos,
                       AListBase& other,
                       Iterator first,
                       Iterator last) {

    if (&other != this) {

        Iterator item = first;
        while (item != last) {

            Iterator next = item;
            ++next;
            insert(pos, other.remove(item));
            item = next;
        }
    }
}

