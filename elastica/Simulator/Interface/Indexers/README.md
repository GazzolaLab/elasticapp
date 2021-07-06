## Indexers

This folder has functionality to index into systems, abstracting from the actual number of indices, in `Elastica++`. The
functionality is split into multiple entities. Symbolic here corresponds to numeral indices or indices from the end.

- [At](At.hpp)
    - A compact set of (symbolic) indices, useful in constraining/forcing at multiple points.
- [Pair](Pair.hpp)
    - A set of two (symbolic) indices. Useful in context where one system connects to another.
- [Range](Range.hpp)
    - A range of (symbolic) indices. Useful when forcing is required over multiple nodes. Can be used to employ
      vectorized algorithms in case it is not strided.
