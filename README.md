# ETL Template Library for C++

## Introduction - Why do we need another xTL?

ETL is a C++ template container library focused on embedded
applications and resource-limited environments.
Its main purpose is to provide _easy to use support_ for
different _allocation strategies_ for containers.
The motivation is to _make the allocation model explicit_,
discoverable and easy to change.
This results in declarations like

```C++
// A vector of Items with reserved capacity of 128 on stack
Etl::Static::Vector<Item, 128> items;
```
or
```C++
// A map of int-Item pairs with a pool allocator of 64 entries
Etl::Pooled::Map<int, Item, 64> items;
```

## Features

It provides

- _Array, Vector, List, Map, Multimap, Set_ types
- interfaces and behavioral concepts mostly compatible
with _stl_ - it works as a drop-in replacement for
most use cases
- improved interoperability - e.g. `swap()` works between
different allocators as a fallback to _move/copy_
- C++ 03 support, no exceptions

It's important to note that _ETL is not a standard compilant
stl implementation_ and it will never be.

---

More info coming soon. Check out and try it until then,
pull requests are welcome.
