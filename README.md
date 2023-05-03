# ETL Template Library for C++

![build](https://github.com/baltth/ETL/actions/workflows/main.yml/badge.svg)

## Introduction - Why do we need another xTL?

ETL is a C++ template container library focused on embedded
applications and resource-limited environments.
Its main purpose is to provide _easy to use support_ for
different _allocation strategies_ for containers.
The motivation is to _make the allocation model explicit_,
discoverable and easy to change.
This results in declarations like

```C++
// A vector of integers with reserved capacity of 128 on stack
Etl::Static::Vector<int, 128> items;
```
or
```C++
// A map of int-Item pairs with a pool allocator of 64 entries
Etl::Pooled::Map<int, Item, 64> items;
```
or
```C++
// A list of integers with heap allocation 
Etl::Dynamic::List<int> items;
```

The user code of the containers doesn't have to care about the allocation details, just use like
```C++
void fill(Etl::Vector<int>& items) {
    items = { 1, 2, 3, 4, 5, 6 };
}

Etl::Static::Vector<int, 128> staticItems;
fill(staticItems);

Etl::Dynamic::Vector<int> dynItems;
fill(dynItems);
```

Even _move, swap,_ etc. works with this interface:
```C++
void consume(Etl::Vector<int>& items) {
    Etl::Dynamic::Vector<int> consumer = std::move(items);
    // This operation falls back to move-by-element when container storage move is
    // not possible like 'Static -> Dynamic' cases
}
```

## Features

It provides

- _Vector, List, Map, MultiMap, Set, UnorderedMap, UnorderedSet, Span_ types
  - and some extra like a lightweight stream-to-buffer utility
- interfaces and behavioral concepts mostly compatible
  with _STL_ - it works as a drop-in replacement for
  most use cases
- improved interoperability - e.g. `swap()` works between
  different allocators as a fallback to _move/copy_
- C++ 11 support, no exceptions

It's important to note that _ETL is not a standard compliant
STL implementation_ and it will never be.

### Allocation

There are four specific allocation strategies available for the containers:
- `Static` strategy defines the storage along with the _actual instance_ with
  a parametrized element number
- `Pooled` strategy defines a common pool for the _parametrized container type_.
  This actually means that all `Etl::Pooled::List<int, 32U>` instances will use
  the same pool, but it's independent of the pool for `Etl::Pooled::List<int, 48U>`
  instances.
- `Custom` strategy allows to use an `std::allocator`-compliant allocator
- `Dynamic` strategy uses `std::allocator`

### Containers

ETL provides the following containers:
- `Vector`
- `List`
- `Map`
- `MultiMap`
- `Set`
- `UnorderedMap`
- `UnorderedMultiMap`
- `UnorderedSet`
- `Array` as an alias to `std::array`

> Note: `Deque` and `MultiSet` and `UnorderedMultiSet` may be added later
> but handled as low priority.

All containers can be used with all strategies except
- `Vector` using only `Static`, `Dynamic` and `Custom`
- `Array` as it's fixed size...

### Utilities

- `Span` is just like `std::span`, backported to C++11
- `MemoryPool` is a general-purpose pool for fixed-size raw memory
- `BufStr` is a `char` buffer with stream-like interface
- `Fifo` and `FifoAccess` are container adaptors for circular
  buffer use

## Building the library

ETL uses [CMake](https://cmake.org/documentation/) build system generator, used the ordinary way:
- configure with `cmake -B $BUILD_DIR -DCMAKE_BUILD_TYPE=Release`,
  e.g. to build into `build` folder:
  ```
  $ cmake -B build -DCMAKE_BUILD_TYPE=Release
  ```
- build with `cmake --build $BUILD_DIR`
- install with `cmake --install $BUILD_DIR` on demand
  > Use the `-DCMAKE_INSTAL_PREFIX=...` switch to specify installation path when configuring.

Note that **`CMAKE_BUILD_TYPE` has to be specified** when configuring, feel free to use any builtin type.

### Building as external project

The project can be used as a CMake [external project](https://cmake.org/cmake/help/latest/module/ExternalProject.html),
this is the preferred way of using in another CMake project.

### Direct use

The library can be built directly in any project with including
`libetl.cmake`. Any compiler setup can be passed with defining
public and private _compile interface targets_
(`etl-compile-if` and `etl-prv-compile-if`), preferably
as an `INTERFACE` library. See `CMakeLists.txt` for an example.

---

More info coming soon. Check out and try it until then,
_pull requests are welcome_.
