/**
\file
\date 2016.01.20. 14:13:53
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

#ifndef __TYPEDARRAYBASE_H__
#define __TYPEDARRAYBASE_H__

#include "EFpp/Utility/Container/Base/AArrayBase.h"


template<typename T>
class TypedArrayBase : public AArrayBase {

// types
public:

	typedef T* Iterator;

// functions
public:

	TypedArrayBase(void* dataPointer, uint32_t len) :
		AArrayBase(sizeof(T), dataPointer, len) {};

	inline T &operator[](uint32_t ix) {
		return *(static_cast<T*>(data) + ix);
	}

	inline const T &operator[](uint32_t ix) const {
		return *(static_cast<T*>(data) + ix);
	}

	inline Iterator begin() const {
		return static_cast<Iterator>(data);
	}

	inline Iterator end() const {
		return static_cast<Iterator>(getItemPointer(capacity));
	}

	inline T* getData() {
		return static_cast<T*>(data);
	}

	inline const T* getData() const {
		return static_cast<T*>(data);
	}

	void fill(const T &value);

};


template<typename T>
void TypedArrayBase<T>::fill(const T &value) {

	for(uint32_t i = 0; i < getSize(); ++i) {
		operator[](i) = value;
	}
}

#endif /* __TYPEDARRAYBASE_H__ */
