//
// Created by Tejaswin Parthasarathy on 1/20/21.
//

#include <type_traits>

#include "Utilities/TypeTraits/IsDetected.hpp"
#include "Utilities/NoSuchType.hpp"

/// [is_detected_example]
template <class T>
using copy_assign_t = decltype(std::declval<T&>() = std::declval<const T&>());
struct Meow {};
static_assert(tt::is_detected<copy_assign_t, Meow>::value,
              "Meow should be copy assignable!");
/// [is_detected_example]

/// [is_detected_v_example]
struct Purr {
  void operator=(const Purr&) = delete;
};
static_assert(tt::is_detected_v<copy_assign_t, Purr> ==
                  tt::is_detected<copy_assign_t, Purr>::value,
              "Purr should not be copy assignable!");
static_assert(not tt::is_detected_v<copy_assign_t, Purr>,
              "Purr should not be copy assignable!");
/// [is_detected_v_example]

/// [detected_or_t_example]
template <class T>
using diff_t = typename T::difference_type;

template <class Ptr>
using difference_type = tt::detected_or_t<std::ptrdiff_t, diff_t, Ptr>;

struct Woof {
  using difference_type = int;
};
struct Bark {};

static_assert(std::is_same<difference_type<Woof>, int>::value,
              "Woof's difference_type should be int!");
static_assert(std::is_same<difference_type<Bark>, std::ptrdiff_t>::value,
              "Bark's difference_type should be ptrdiff_t!");
/// [detected_or_t_example]

/// [detected_t_example]
template <class T>
using add_t = typename T::addition_type;

struct C {
  using addition_type = int;
};
struct D {};

static_assert(std::is_same<tt::detected_t<add_t, C>, int>::value,
              "C's difference_type should be int!");
static_assert(std::is_same<tt::detected_t<add_t, D>, NoSuchType>::value,
              "D should not have a difference type!");
/// [detected_t_example]
