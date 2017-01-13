/**
\file
\date 2016.01.20. 19:39:42
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

#ifndef __LISTTEMPLATE_H__
#define __LISTTEMPLATE_H__

#undef min
#undef max

#include <utility>
#include <functional>
#include <initializer_list>

#include "EFpp/Utility/Container/Base/AListBase.h"


template<class T>
class ListTemplate : protected AListBase {

// types
public:

	class Node : public AListBase::Node {

	public:

		T item;

		template<typename... Args>
		Node(Args &&... args) :
			item(std::forward<Args>(args)...) {};

	};

	class Iterator : public AListBase::Iterator {
		friend class ListTemplate<T>;

	public:

		T &operator*() const {
			return static_cast<ListTemplate<T>::Node*>(node)->item;
		};

		T* operator->() const {
			return &(static_cast<ListTemplate<T>::Node*>(node)->item);
		};

		bool operator==(const Iterator &other) {
			return AListBase::Iterator::operator==(other);
		}

		bool operator!=(const Iterator &other) {
			return !(operator==(other));
		}

		Iterator &operator++() {
			AListBase::Iterator::operator++();
			return *this;
		}

		Iterator &operator--() {
			AListBase::Iterator::operator--();
			return *this;
		}

		const Iterator operator++(int) {
			Iterator old = *this;
			this->operator++();
			return old;
		}

		const Iterator operator--(int) {
			Iterator old = *this;
			this->operator--();
			return old;
		}

		Iterator(const AListBase::Iterator &it) :
			AListBase::Iterator(it) {};

	protected:

		Iterator(ListTemplate<T>::Node* n) :
			AListBase::Iterator(n) {};

	};

// functions
public:

	ListTemplate() = default;

	ListTemplate(ListTemplate &&other) :
		AListBase(std::move(other)) {};

	ListTemplate &operator=(ListTemplate &&other) {
		AListBase::operator=(other);
		return *this;
	}

	ListTemplate(const std::initializer_list<T> &initList);

	~ListTemplate() {
		clear();
	}

	void clear();

	///\name Általános AListBase forward
	/// @{
	inline uint32_t getSize() const {
		return AListBase::getSize();
	}

	inline Iterator begin() const {
		return Iterator(AListBase::begin());
	}

	inline Iterator end() const {
		return Iterator(AListBase::end());
	}
	/// @}

	virtual T* createItem() {
		return nullptr;
	}

	/// \name Műveletek elemekkel
	/// @{
	inline void pushFront(const T &item) {
		return AListBase::pushFront(new Node(item));
	}

	inline void pushBack(const T &item) {
		return AListBase::pushBack(new Node(item));
	}

	inline void popFront() {
		delete AListBase::popBack();
	}

	inline void popBack() {
		delete AListBase::popBack();
	}

	inline Iterator insert(Iterator pos, const T &item) {
		return emplace(pos, item);
	}

	template<typename... Args >
	Iterator emplace(Iterator pos, Args &&... args);

	inline void erase(Iterator pos) {
		delete AListBase::remove(pos);
	}
	/// @}

	Iterator find(std::function<bool(const T &)> &&matchCall) const {
		return find(begin(), end(), std::move(matchCall));
	}

	Iterator find(Iterator startPos, Iterator endPos, std::function<bool(const T &)> &&matchCall) const;

	template<typename F, typename V>
	Iterator find(F f, const V &v) const {
		return find(begin(), end(), f, v);
	}

	template<typename F, typename V>
	Iterator find(Iterator startPos, Iterator endPos, F f, const V &v) const;

};

template<class T>
ListTemplate<T>::ListTemplate(const std::initializer_list<T> &initList) {

	for(auto &it : initList) {
		pushBack(it);
	}
}


template<class T>
void ListTemplate<T>::clear() {

	while(getSize() > 0) {
		popBack();
	}
}


template<class T>
template<typename... Args >
typename ListTemplate<T>::Iterator ListTemplate<T>::emplace(Iterator pos, Args &&... args) {

	Node* inserted = new Node(std::forward<Args>(args)...);
	AListBase::insert(pos, inserted);
	return Iterator(inserted);
}


template<class T>
typename ListTemplate<T>::Iterator ListTemplate<T>::find(Iterator startPos,
														 Iterator endPos,
														 std::function<bool(const T &)> &&matchCall) const {

	bool match = false;

	while(!match && (startPos != endPos)) {

		match = matchCall(*startPos);

		if(!match) {
			++startPos;
		}
	}

	return startPos;
}


template<class T>
template<typename F, typename V>
typename ListTemplate<T>::Iterator ListTemplate<T>::find(Iterator startPos,
														 Iterator endPos,
														 F f,
														 const V &v) const {

	bool match = false;

	while(!match && (startPos != endPos)) {

		match = (((*startPos).*f)() == v);

		if(!match) {
			++startPos;
		}
	}

	return startPos;
}

#endif /* __LISTTEMPLATE_H__ */
