//
// Created by Tejaswin Parthasarathy on 5/26/21.
//

#include <type_traits>

#include "Systems/States/TypeTraits/IsVectorized.hpp"

// [is_vectorized_eg]
struct IndicatesVectorized {
  using is_vectorized = std::true_type;
};

struct IndicatesNotVectorized {
  using is_vectorized = std::false_type;
};

struct DoesNotIndicateVectorized {
  // no is_vectorized typedef
  // using is_vectorized = std::false_type;
};

static_assert(::elastica::states::tt::IsVectorized<IndicatesVectorized>::value,
              "IsVectorized failure!");
static_assert(
    not ::elastica::states::tt::IsVectorized<IndicatesNotVectorized>::value,
    "IsVectorized failure!");
static_assert(
    not ::elastica::states::tt::IsVectorized<DoesNotIndicateVectorized>::value,
    "IsVectorized failure!");
// [is_vectorized_eg]
