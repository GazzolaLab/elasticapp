//
// Created by Tejaswin Parthasarathy on 5/28/21.
//
#include <type_traits>

#include "Systems/States/Expressions/OrderTags.hpp"
#include "Systems/States/States.hpp"
#include "Systems/States/TypeTraits/IsPrimitive.hpp"

namespace {

  /// [is_primitive_eg]
  using namespace elastica::states;
  using namespace elastica::states::tt;
  static_assert(IsPrimitive<SO3<double, tags::PrimitiveTag> >::value,
                "IsPrimitive failure!");
  static_assert(IsPrimitive<SE3<float, tags::PrimitiveTag> >::value,
                "IsPrimitive failure!");
  static_assert(not IsPrimitive<SO3<float, tags::DerivativeTag> >::value,
                "IsPrimitive failure!");
  static_assert(IsNotPrimitive<SE3<int, tags::DoubleDerivativeTag> >::value,
                "IsPrimitive failure!");
  static_assert(is_primitive_v<SO3<float, tags::PrimitiveTag> >,
                "IsPrimitive failure!");
  /// [is_primitive_eg]

}  // namespace
