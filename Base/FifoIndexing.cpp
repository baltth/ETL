/**
\file
\date 2016.01.02. 18:58:16
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

#include "FifoIndexing.h"


void FifoIndexing::resetIndexes() {

	writeIx = 0;
	readIx = 0;
	numItems = 0;
}


uint32_t FifoIndexing::getIndexFromFront(uint32_t ix) const {

	uint32_t bufferIx = nextIndex(readIx) + limitIndexForNumItems(ix);

	if(bufferIx >= getCapacity()) {
		bufferIx -= getCapacity();
	}

	return bufferIx;
}


uint32_t FifoIndexing::getIndexFromBack(uint32_t ix) const {

	int32_t bufferIx = writeIx - limitIndexForNumItems(ix);

	if(bufferIx < 0) {
		bufferIx += getCapacity();
	}

	return bufferIx;
}


uint32_t FifoIndexing::limitIndexForNumItems(uint32_t ix) const {

	if(ix >= getNumItems()) {
		if(getNumItems() == 0) {
			ix = 0;
		} else {
			ix = getNumItems() - 1;
		}
	}

	return ix;
}


uint32_t FifoIndexing::nextIndex(uint32_t ix) const {

	++ix;

	if(ix >= getCapacity()) {
		ix = 0;
	}

	return ix;
}


uint32_t FifoIndexing::previousIndex(uint32_t ix) const {

	if((ix == 0) && (getCapacity() > 0)) {
		ix = getCapacity() - 1;
	} else {
		--ix;
	}

	return ix;
}

void FifoIndexing::push() {

	writeIx = nextIndex(writeIx);

	if(writeIx == readIx) {
		readIx = nextIndex(readIx);
	} else {
		++numItems;
	}

}

void FifoIndexing::pop() {

	if(numItems > 0) {
		readIx = nextIndex(readIx);
		--numItems;
	}
}
