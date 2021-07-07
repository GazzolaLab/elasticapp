//
// Created by Tejaswin Parthasarathy on 5/30/21.
//
#include <type_traits>

#include "Systems/States/Expressions/SO3/SO3.hpp"
#include "Systems/States/Expressions/States/StatesStatesAddExpr.hpp"
#include "Systems/States/States.hpp"
#include "Systems/States/TypeTraits/IsScalar.hpp"

namespace {

  /// [is_scalar_eg]
  using namespace elastica::states;
  using namespace elastica::states::tt;

  // doubles and floats are scalars
  static_assert(IsScalar<double>::value, "IsScalar failure!");
  static_assert(is_scalar_v<floats>::value, "IsScalar failure!");

  // states are not scalars
  static_assert(not IsScalar<SO3<float, tags::DerivativeTag>>::value,
                "IsScalar failure!");
  static_assert(not IsScalar<SE3<float, tags::DoubleDerivativeTag>>::value,
                "IsScalar failure!");

  // expressions are not scalars either
  using TestType = StatesStatesAddExpr<States<int>, States<float>>;
  static_assert(not IsScalar<TestType>::value, "IsScalar failure!");

  // pointers are not scalars
  static_assert(not IsScalar<double*>::value, "IsScalar failure!");
  // neither are references
  static_assert(not is_scalar_v<float&>, "IsScalar failure!");
  /// [is_scalar_eg]

}  // namespace
