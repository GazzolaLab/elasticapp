//
// Created by Tejaswin Parthasarathy on 5/30/21.
//
#include "Systems/States/Expressions/OrderTags/TypeTraits.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {

  // [lower_order_t_eg]
  struct IGNORE_UNUSED PrimitiveState {
    // mimics a state
    using Order = elastica::states::tags::PrimitiveTag;
  };
  // compilation error, cannot lower primitives
  //  static_assert(
  //      std::is_same_v<elastica::states::tt::lower_order_t<PrimitiveState>,
  //      elastica::states::tags::internal::PrimitiveTag,
  //      >, "Lower order failure!");

  struct DerivativeState {
    // mimics a state
    using Order = elastica::states::tags::DerivativeTag;
  };
  static_assert(
      std::is_same_v<
          elastica::states::tt::lower_order_t<DerivativeState>,
          elastica::states::tags::internal::DerivativeMultipliedByTimeTag>,
      "Lower order failure!");

  struct DoubleDerivativeState {
    // mimics a state
    using Order = elastica::states::tags::DoubleDerivativeTag;
  };
  static_assert(std::is_same_v<
                    elastica::states::tt::lower_order_t<DoubleDerivativeState>,
                    elastica::states::tags::internal::
                        DoubleDerivativeMultipliedByTimeTag>,
                "Lower order failure!");
  // [lower_order_t_eg]

  // [common_order_t_eg]
  struct One {
    using Order =
        elastica::states::tags::internal::DoubleDerivativeMultipliedByTimeTag;
  };
  struct Two {
    using Order = elastica::states::tags::DerivativeTag;
  };
  static_assert(std::is_same_v<elastica::states::tt::common_order_t<One, Two>,
                                 elastica::states::tags::internal::
                                     DoubleDerivativeMultipliedByTimeTag>,
                "Common order failure!");
  // [common_order_t_eg]
  struct Three {
    using Order =
        elastica::states::tags::internal::DerivativeMultipliedByTimeTag;
  };
  static_assert(std::is_same_v<elastica::states::tt::common_order_t<Two, One>,
                                 elastica::states::tags::internal::
                                     DoubleDerivativeMultipliedByTimeTag>,
                "Common order failure!");
  static_assert(std::is_same_v<elastica::states::tt::common_order_t<One, One>,
                                 elastica::states::tags::internal::
                                     DoubleDerivativeMultipliedByTimeTag>,
                "Common order failure!");
  static_assert(
      std::is_same_v<
          elastica::states::tt::common_order_t<Three, Three>,
          elastica::states::tags::internal::DerivativeMultipliedByTimeTag>,
      "Common order failure!");

}  // namespace