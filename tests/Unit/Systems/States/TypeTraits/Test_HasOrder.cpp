//
// Created by Tejaswin Parthasarathy on 5/29/21.
//

#include <type_traits>
#include <utility>

#include "Systems/States/Expressions/OrderTags.hpp"
#include "Systems/States/States.hpp"
#include "Systems/States/TypeTraits/HasOrder.hpp"
#include "Time/SimulationTime.hpp"

namespace {

  /// [has_order_eg]
  using namespace elastica::states;
  using namespace elastica::states::tt;

  // Works for a simple state marked as SO3 or SE3
  static_assert(HasOrder<SO3<float, tags::DerivativeTag>>::value,
                "HasOrder failure!");
  static_assert(HasOrder<SE3<double, tags::DoubleDerivativeTag>>::value,
                "HasOrder failure!");

  // Works for an expression of states
  using Derivative = SO3<float, tags::DerivativeTag>;
  using DoubleDerivative = SO3<float, tags::DoubleDerivativeTag>;
  using TD = elastica::TimeDelta;
  using TestType =
      decltype(std::declval<Derivative>() +
               std::declval<TD>() * std::declval<DoubleDerivative>());
  static_assert(HasOrder<TestType>::value, "HasOrder failure!");

  // Works for other types
  struct AnotherTestType {
    // using order = NOOrderTypedef;
  };
  static_assert(not HasOrder<AnotherTestType>::value, "HasOrder failure!");
  /// [has_order_eg]

}  // namespace
