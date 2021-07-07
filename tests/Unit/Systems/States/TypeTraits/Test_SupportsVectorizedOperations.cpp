//
// Created by Tejaswin Parthasarathy on 5/26/21.
//

#include <type_traits>

#include "Systems/States/TypeTraits/SupportsVectorizedOperations.hpp"

// [supports_vectorized_eg]
struct SupportsVectorizedOperations {
  auto operator+=(SupportsVectorizedOperations const&)
      -> SupportsVectorizedOperations&;
};
auto operator+(SupportsVectorizedOperations const&,
               SupportsVectorizedOperations const&)
    -> SupportsVectorizedOperations;
auto operator*(SupportsVectorizedOperations const&, double)
    -> SupportsVectorizedOperations;
auto operator*(double, SupportsVectorizedOperations const&)
    -> SupportsVectorizedOperations;

static_assert(::elastica::states::tt::SupportsVectorizedOperations<
                  SupportsVectorizedOperations>::value,
              "IsVectorizedOperations failure!");

struct DoesNotSupportVectorizedOperations {};
static_assert(not ::elastica::states::tt::SupportsVectorizedOperations<
                  DoesNotSupportVectorizedOperations>::value,
              "SupportsVectorizedOperations failure!");

struct PartiallySupportsVectorizedOperations {};
auto operator+(PartiallySupportsVectorizedOperations const&,
               PartiallySupportsVectorizedOperations const&)
    -> PartiallySupportsVectorizedOperations;
auto operator*(PartiallySupportsVectorizedOperations const&, double)
    -> PartiallySupportsVectorizedOperations;
// left multiplication is intentionally not defined
// operator*(double, PartiallySupportsVectorizedOperations const&)
//    ->PartiallySupportsVectorizedOperations;
static_assert(not ::elastica::states::tt::SupportsVectorizedOperations<
                  PartiallySupportsVectorizedOperations>::value,
              "SupportsVectorizedOperations failure!");
// [supports_vectorized_eg]

// explicitly check blaze types as these are our bread and butter
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/DynamicVector.h>
#include <blaze_tensor/math/DynamicTensor.h>

static_assert(::elastica::states::tt::SupportsVectorizedOperations<
                  blaze::DynamicMatrix<double>>::value,
              "IsVectorizedOperations failure!");
static_assert(::elastica::states::tt::SupportsVectorizedOperations<
                  blaze::DynamicVector<double>>::value,
              "IsVectorizedOperations failure!");
static_assert(::elastica::states::tt::SupportsVectorizedOperations<
                  blaze::DynamicTensor<double>>::value,
              "IsVectorizedOperations failure!");
