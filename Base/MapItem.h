/**
\file
\date 2015. 05. 27. 13:32:30
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

#ifndef __ETL_MAPITEM_H__
#define __ETL_MAPITEM_H__

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {


template<typename K, class E>
class MapItem {

// variables
private:

    const K key;
    mutable E element;

// functions
public:

    explicit MapItem<K, E>(const K &k) :
        key(k) {};

    MapItem<K, E>(const K &k, const E &e) :
        key(k),
        element(e) {};

    template<typename... Args>
    MapItem<K, E>(const K &k, Args &&... args) :
        key(k),
        element(std::forward<Args>(args)...) {};

    K getKey() const {
        return key;
    }

    E &getElement() const {
        return element;
    }

    void setElement(const E &newElement) {
        element = newElement;
    }

    void setElement(E &&newElement) {
        element = std::move(newElement);
    }

    bool operator<(const MapItem<K, E> &other) const {
        return (key < other.key);
    }

};


template<typename K, class E>
class MapItem<K, E*> {

// variables
protected:

    const K key;
    mutable E* element;

// functions
public:

    explicit MapItem<K, E*>(const K &k) :
        key(k),
        element(nullptr) {};

    MapItem<K, E*>(const K &k, E* e) :
        key(k),
        element(e) {};

    K getKey() const {
        return key;
    }

    E* &getElement() const {
        return element;
    }

    void setElement(E* newElement) {
        element = newElement;
    }

};


template<typename K, class E>
bool operator<(const MapItem<K, E*> &lhs, const MapItem<K, E*> &rhs) {

    return (lhs.getKey() < rhs.getKey());
}

}

#endif /* __ETL_MAPITEM_H__ */
