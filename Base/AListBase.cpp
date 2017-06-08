﻿/**
\file
\date 2015.05.21.
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

#include "Base/AListBase.h"

using namespace ETL_NAMESPACE;


AListBase::Node* AListBase::popFront() {

    if (list.getSize() > 0) {
        return static_cast<Node*>(list.remove(list.getFirst()));
    } else {
        return NULLPTR;
    }
}

AListBase::Node* AListBase::popBack() {

    if (list.getSize() > 0) {
        return static_cast<Node*>(list.remove(list.getLast()));
    } else {
        return NULLPTR;
    }
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


