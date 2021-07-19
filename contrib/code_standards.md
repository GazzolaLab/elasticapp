# Introduction

On this page one can read about all the code standards that are used in `elastica++`. The page is intended to aid new
developers conform with the `elastica++` code standards. Issues such as style and layout are often subjective, so bear
in mind that all statements on this page is the collective opinion of the `elastica++` developers only.

# Formating and Naming Conventions

## Clang-format

Please utilize `clang-format` along with the configuration file shipped with the software for all code-formatting.

## Avoid the typedef keyword

To declare type-definitions adopt the `using` keyword rather than the `typedef` keyword. Here is a small code example
showing use with the `using` keyword.

```cpp
...
// STL iterator requirements
//! The iterator category.
using iterator_category = std::random_access_iterator_tag;
//! Type of the underlying elements.
using value_type = int;
//! Pointer return type.
using pointer = int*;
//! Reference return type.
using reference = int&;
//! Difference between two iterators.
using difference_type = std::ptrdiff_t;
...
```

## Type-definitions placement

All typedefs should appear in the top-most position and preferably be collected in contiguous blocks of logic coherence.

Here is a small example

```cpp
    template<typename math_types>
    class MyCalculator
    {
      public:
      using real_type = typename math_types::real_type;
      using size_type = typename math_types::size_type;

      // ... details
    };

    template<typename point_container>
    void do_iterate(point_container const & points)
    {
      using point_type = typename point_container::value_type;
      using point_reference = typename point_container::reference_type;

      for( auto p = points.begin(); p != points.end(); ++p)
      {
        auto q = static_cast<point_type&>(*p);
        auto same_q = static_cast<point_reference>(*p);
        // details...
      }
    }
```

## Const Placement

In `elastica++` we use the east `const` convention

```cpp
int const &
```

rather than

```cpp
const int const &
```

The reason for this convention is firstly of all to be consistent to make all `elastica++` code have the same style,
look and feel. Secondly we currently believe this convention is better for readability and understanding of the code.
Here are more illustrative examples

```cpp
int const *
```

Reading from right to left we immediately see this is a pointer to a constant integer. Whereas

```cpp
int * const
```

is a constant pointer to an integer. With our choice of convention one only have to remember that the `const` keyword
tells something about the token immediately to the left of it.

## Template typename/class keyword

In templates we prefer the keyword `typename` over the keyword `class`.

In cases where it is necessary to use template-template arguments, the keyword `class` will have to be used.

A minimal example:

```cpp
template< template <typename T> class T2>
class SomeClass {
// ... details
};
```

## Classes, Members and Function Naming Conventions

In order to ensure a consists look and feel we have decided upon a specific formatting for names.

* The first letter in each word of a class name must be written with uppercase. The remaining letters must be written in
  lowercase.
* If the class name consists of several words then these are simply concatenated.
* All data members within a class must have a string suffix "_"
* All data members must be written in lower-case. If names consist of several words then these must be concatenated
  using an underscore
* All methods must be written in lower-case. If names consist of several words then these must be concatenated using an
  underscore
* All functions must be written in lower-case. If names consist of several words then these must be concatenated using
  an underscore

Here is a small example

```cpp
class MyClass
{
public:

  int my_member_;

public:

  void my_method () {}

};

void my_function () {}
```

## Expose Template Parameter Names

If a template argument should be used elsewhere in the code then the template argument should immediately be aliased
inside the class with the "real" name one want to use and refer to later on in the code.

Here is an example of a class that needs a template argument.

```cpp
template <typename math_types_>
class MyClass
{
  public:

  using math_type = math_types_;

  // ... a bunch of other stuff ...
};
```

The little implementation trick ensures that descendants of the class can know about the math types or some algorithm
can know about the math types for generic code.