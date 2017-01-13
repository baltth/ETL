/**
\file
\date 2015.08.03. 12:25:42
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

#ifndef __AARRAYBASE_H__
#define __AARRAYBASE_H__

#include <cstddef>
#include <cstdint>


class AArrayBase {

// types
public:

	typedef void* ItemType;

// variables
protected:

	void* data;
	uint32_t capacity;
	const size_t itemSize;

// functions
public:

	AArrayBase(const AArrayBase &other) = delete;
	AArrayBase &operator=(const AArrayBase &other) = delete;

	void* getItemPointer(uint32_t ix) const {
		return reinterpret_cast<void*>(reinterpret_cast<uint32_t>(data) + (ix * itemSize));
	}

	void* getData() {
		return data;
	}

	const void* getData() const {
		return data;
	}

	uint32_t getSize() const {
		return capacity;
	}

	uint32_t getItemSize() const {
		return itemSize;
	}

protected:

	AArrayBase(size_t itemSize, void* dataPointer, uint32_t len) :
		data(dataPointer),
		capacity(len),
		itemSize(itemSize) {};

};

#endif /* __AARRAYBASE_H__ */
