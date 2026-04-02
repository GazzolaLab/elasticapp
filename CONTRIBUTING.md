# Contributing

Contributions are welcome, although the response may be slow.

## Pull Requests

- Open pull requests against the `main` branch.
- Make sure the relevant CI checks pass before requesting review.
- Keep documentation in sync with code changes when behavior or interfaces
  change.

## Development Guidelines

This page describes the code standards used in `elastica++`. It is intended to
help new developers follow the existing conventions in the codebase. Issues
such as style and layout are often subjective, so treat the guidance below as
the collective opinion of the `elastica++` developers.

## Formatting and Naming Conventions

### Clang-format

Please use `clang-format` together with the configuration file shipped with the
repository for all C++ code formatting.

- The repository configuration lives in
  [`.clang-format`](/Users/skim0119/github/elasticapp/.clang-format).
- A typical invocation is:

```bash
clang-format -i -style=file path/to/file.cpp
```

### CMake Format

Please use `cmake-format` together with the configuration file shipped with the
repository for CMake formatting.

- The repository configuration lives in
  [`.cmake-format.yaml`](/Users/skim0119/github/elasticapp/.cmake-format.yaml).
- A typical invocation is:

```bash
cmake-format -i path/to/CMakeLists.txt
```

### Avoid the `typedef` Keyword

Use `using` rather than `typedef` when declaring type definitions.

```cpp
// STL iterator requirements
using iterator_category = std::random_access_iterator_tag;
using value_type = int;
using pointer = int*;
using reference = int&;
using difference_type = std::ptrdiff_t;
```

### Type Definition Placement

Type aliases should appear near the top of a class or logical scope and should
preferably be collected in contiguous, coherent blocks.

```cpp
template <typename math_types>
class MyCalculator {
 public:
  using real_type = typename math_types::real_type;
  using size_type = typename math_types::size_type;

  // ... details
};

template <typename point_container>
void do_iterate(point_container const& points) {
  using point_type = typename point_container::value_type;
  using point_reference = typename point_container::reference_type;

  for (auto p = points.begin(); p != points.end(); ++p) {
    auto q = static_cast<point_type&>(*p);
    auto same_q = static_cast<point_reference>(*p);
    // details...
  }
}
```

### Const Placement

In `elastica++`, we use the east-`const` convention:

```cpp
int const&
```

rather than:

```cpp
const int&
```

The goal is to keep the codebase consistent and, in our view, easier to read.
For example:

```cpp
int const*
```

is a pointer to a constant integer, whereas:

```cpp
int* const
```

is a constant pointer to an integer.

If you have written code with west-`const` and want to convert it to
`elastica++` conventions, take a look at
[westerly](https://github.com/biasmv/westerly).

### Template `typename` / `class` Keyword

In templates, we prefer the keyword `typename` over the keyword `class`.

In cases where template-template arguments are needed, the keyword `class` must
be used.

```cpp
template <template <typename T> class T2>
class SomeClass {
  // ... details
};
```

### Classes, Members, and Function Naming Conventions

To ensure a consistent look and feel:

- The first letter of each word in a class name should be uppercase.
- If a class name consists of several words, they should be concatenated.
- Data members within a class should use a trailing underscore.
- Data members should be lowercase, with words separated by underscores.
- Methods should be lowercase, with words separated by underscores.
- Free functions should be lowercase, with words separated by underscores.

```cpp
class MyClass {
 public:
  int my_member_;

 public:
  void my_method() {}
};

void my_function() {}
```

### Expose Template Parameter Names

If a template argument is used elsewhere in the code, alias it immediately
inside the class with the semantic name you want to use later.

```cpp
template <typename math_types_>
class MyClass {
 public:
  using math_type = math_types_;

  // ... a bunch of other stuff ...
};
```

This pattern makes it easier for descendants of the class, or external generic
algorithms, to refer to the relevant type meaningfully.
