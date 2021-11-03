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

Even _move, swap,_ etc works with this interface:
```C++
void consume(Etl::Vector<int>& items) {
    Etl::Dynamic::Vector<int> consumer = std::move(items);
    // These operations fall back to copy when move is
    // not possible like 'Static -> Dynamic' cases
}
```

## Features

It provides

- _Array, Vector, List, Map, MultiMap, Set, Span_ types
  - and some extra like a lightweight stream to buffer
- interfaces and behavioral concepts mostly compatible
  with _STL_ - it works as a drop-in replacement for
  most use cases
- improved interoperability - e.g. `swap()` works between
  different allocators as a fallback to _move/copy_
- C++ 11 support, no exceptions

It's important to note that _ETL is not a standard compliant
STL implementation_ and it will never be.

---

More info coming soon. Check out and try it until then,
pull requests are welcome.
