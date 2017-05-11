/**
\file
\date 2017.02.15.
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

#ifndef __ETL_EXCEPTIONTYPES_H__
#define __ETL_EXCEPTIONTYPES_H__

#include "etlSupport.h"

#if ETL_USE_EXCEPTIONS

#include <exception>


namespace ETL_NAMESPACE {

class Exception : public std::exception {

  private:  // variables

    const char* const text;

  public:   // functions

    explicit Exception(const char* t) :
        text(t) {};

    virtual const char* what() const throw () {
        return text;
    }

};


class UnknownException : public Exception {

  public:   // functions

    UnknownException() :
        Exception("") {};

};


class OutOfRangeException : public Exception {

  public:   // functions

    OutOfRangeException() :
        Exception("Index out of range") {};

};

}

#endif

#endif /* __ETL_EXCEPTIONTYPES_H__ */

