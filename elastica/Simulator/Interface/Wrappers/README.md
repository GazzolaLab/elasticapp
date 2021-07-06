## Wrappers

Defines wrappers (i.e. take the underlying object and expose it in a new interface). Useful to curry functions but with
a different call operator. This finds applications is building robust interfaces.

- [AtWrapper](AtWrapper.hpp)
    - Exposes a callable `operator()` using the `at()` syntax.
- [ToWrapper](ToWrapper.hpp)
    - Exposes a callable `operator()` using the `to()` syntax.
- [WithWrapper](WithWrapper.hpp)
    - Exposes a callable `operator()` using the `with()` syntax.
