//
// Created by Tejaswin Parthasarathy on 1/21/21.
//

#include "Utilities/TypeTraits/Cpp17.hpp"

static_assert(true, "Ay!");

/// [conjunction_example]
static_assert(
    std::conjunction<std::true_type, std::true_type, std::true_type>::value,
    "Failed testing type trait conjunction");
static_assert(not std::conjunction<std::true_type, std::false_type,
                                     std::true_type>::value,
              "Failed testing type trait conjunction");
static_assert(not std::conjunction<std::true_type, std::false_type>::value,
              "Failed testing type trait conjunction");
static_assert(not std::conjunction<std::false_type, std::true_type>::value,
              "Failed testing type trait conjunction");
/// [conjunction_example]

/// [disjunction_example]
static_assert(not std::disjunction<std::false_type, std::false_type,
                                     std::false_type>::value,
              "Failed testing type trait disjunction");
static_assert(
    std::disjunction<std::false_type, std::true_type, std::false_type>::value,
    "Failed testing type trait disjunction");
static_assert(std::disjunction<std::true_type, std::false_type>::value,
              "Failed testing type trait disjunction");
static_assert(std::disjunction<std::false_type, std::true_type>::value,
              "Failed testing type trait disjunction");
/// [disjunction_example]

/// [negation_example]
static_assert(
    std::is_same<
        std::bool_constant<false>,
        typename std::negation<std::bool_constant<true>>::type>::value,
    "Failed testing type trait negate");
static_assert(
    std::is_same<
        std::bool_constant<true>,
        typename std::negation<std::bool_constant<false>>::type>::value,
    "Failed testing type trait negate");
static_assert(not std::negation<std::bool_constant<true>>::value,
              "Failed testing type trait negate");
static_assert(std::negation<std::bool_constant<false>>::value,
              "Failed testing type trait negate");
/// [negation_example]

/// [is_same_v_example]
static_assert(std::is_same_v<decltype(2 * 3), decltype(4 + 5)>,
              "Failed testing is_same_v");
/// [is_same_v_example]
