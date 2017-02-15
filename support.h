/*
 * Copyright (C) Commsignia Ltd. - All Rights Reserved
 * Unauthorised copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Balazs Toth <balazs.toth@commsignia.com>, 2017
 */

#ifndef __ETL_SUPPORT_H__
#define __ETL_SUPPORT_H__

#ifndef ETL_NAMESPACE
#define ETL_NAMESPACE   Etl
#endif

namespace ETL_NAMESPACE {

#ifdef ETL_USE_CPP11

#if ((ETL_USE_CPP11 > 0) && (__cplusplus < 201103L))
#undef ETL_USE_CPP11
#define ETL_USE_CPP11   0
#endif

#else

#if __cplusplus < 201103L
#define ETL_USE_CPP11   1
#endif

#endif

#if ETL_USE_CPP11

// nullptr idiom from https://en.wikibooks.org/wiki/More_C++_Idioms/nullptr

const // It is a const object...
class NullptrT 
{
  public:
    template<class T>
    inline operator T*() const // convertible to any type of null non-member pointer...
    { return 0; }

    template<class C, class T>
    inline operator T C::*() const   // or any type of null member pointer...
    { return 0; }

  private:
    void operator&() const;  // Can't take address of nullptr

} NULLPTR = {};

#else

#define NULLPTR         nullptr

#endif

}

#endif /* __ETL_SUPPORT_H__ */

