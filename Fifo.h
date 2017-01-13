/**
\file
\date 2015. 07. 02. 12:07:03
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

#ifndef __FIFOVECTOR_H__
#define __FIFOVECTOR_H__

#include "EFpp/Utility/Container/Base/FifoTemplate.h"
#include "EFpp/Utility/Container/Array.h"

/**
\ingroup container
*/
template<class T>
class Fifo : public FifoTemplate<T> {

// types
public:

	typedef typename FifoTemplate<T>::ItemType ItemType;
	typedef typename FifoTemplate<T>::Iterator Iterator;

// functions
public:

	template<typename... Args>
	explicit Fifo(Args &&... args) :
		FifoTemplate<T>(args...) {};

	void setupFifo(uint32_t len);

};


template<class T>
void Fifo<T>::setupFifo(uint32_t len) {

	T::resize(len);
	FifoIndexing::setCapacity(T::getSize());
	FifoIndexing::resetIndexes();
}


template<>
template<typename T, uint32_t N>
class Fifo<Array<T, N>> : public FifoTemplate<Array<T, N>> {

// types
public:

	typedef typename FifoTemplate<Array<T, N>>::ItemType ItemType;
	typedef typename FifoTemplate<Array<T, N>>::Iterator Iterator;

// functions
public:

	template<typename... Args>
	explicit Fifo(Args &&... args) :
		FifoTemplate<Array<T, N>>(args...) {};

	void setupFifo(uint32_t len) {};

};

#endif /* __FIFOVECTOR_H__ */
