/**
\file
\date 2016.01.02. 20:07:16
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

#ifndef __AFIFOITERATOR_H__
#define __AFIFOITERATOR_H__

#include "EFpp/Utility/Container/Base/FifoIndexing.h"


template<class T>
class AFifoIterator {

// types
public:


// variables
private:

	uint32_t ix;
	T* data;
	const FifoIndexing* fifo;

// functions
public:

	AFifoIterator(const AFifoIterator &other) = default;
	AFifoIterator &operator=(const AFifoIterator &other) = default;

	bool operator==(const AFifoIterator &other) const {
		return ((ix == other.ix) && (fifo == other.fifo));
	}

	inline bool operator!=(const AFifoIterator &other) const {
		return (!operator==(other));
	}

	AFifoIterator &operator++() {
		++ix;
		return *this;
	}

	const AFifoIterator &operator++(int) {
		AFifoIterator old = *this;
		this->operator++();
		return old;
	}

	T &operator*() const {
		return *(get());
	};

	T* operator->() const {
		return get();
	};

protected:

	AFifoIterator(T* data, const FifoIndexing* indexing, uint32_t index) :
		ix(index),
		data(data),
		fifo(indexing) {};

	const FifoIndexing* getFifoIndexing() const {
		return fifo;
	}

	inline T* get() const {
		return data + fifo->getIndexFromFront(ix);
	}

};

#endif /* __AFIFOITERATOR_H__ */
