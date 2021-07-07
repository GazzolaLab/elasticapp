//
// Created by Tejaswin Parthasarathy on 5/29/21.
//
#include <type_traits>

#include "Systems/States/Expressions/OrderTags.hpp"
#include "Systems/States/Expressions/SO3/SO3.hpp"
#include "Systems/States/Expressions/States/StatesStatesAddExpr.hpp"
#include "Systems/States/States.hpp"
#include "Systems/States/TypeTraits/IsExpression.hpp"
#include "Time/SimulationTime.hpp"

namespace {

  /// [is_expression_eg]
  using namespace elastica::states;
  using namespace elastica::states::tt;

  // works for an expression of SO3 or SE3
  static_assert(
      IsExpression<
          decltype(std::declval<SO3<double, tags::DerivativeTag>>() +
                   std::declval<elastica::TimeDelta>() *
                       std::declval<
                           SO3<double, tags::DoubleDerivativeTag>>())>::value,
      "IsExpression failure!");
  static_assert(not IsExpression<SE3<float, tags::DerivativeTag>>::value,
                "IsExpression failure!");

  // works for an expression of states too
  struct One {
    using Order = tags::DerivativeTag;
  };
  struct Two {
    using Order = tags::internal::DoubleDerivativeMultipliedByTimeTag;
  };
  using TestType = StatesStatesAddExpr<States<One>, States<Two>>;
  static_assert(is_expression_v<TestType>, "IsExpression failure!");

  // works for other non-expression types
  struct NotAnExpression {};
  static_assert(not is_expression_v<NotAnExpression>, "IsExpression failure!");
  /// [is_expression_eg]

}  // namespace
