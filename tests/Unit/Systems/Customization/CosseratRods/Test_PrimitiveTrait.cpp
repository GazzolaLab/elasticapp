//
// Created by Tejaswin Parthasarathy on 4/25/22.
//

#include "Systems/CosseratRods/CosseratRods.hpp"
#include "Systems/Customization/CosseratRods.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {

  using namespace elastica::cosserat_rod;
  using ::elastica::collision::tt::PrimitiveTrait_t;

  static_assert(
      cpp17::is_same_v<
          ::elastica::collision::tt::PrimitiveTrait_t<detail::CosseratRod>,
          ::elastica::collision::CapsuleFromRodPrimitive>,
      "Meta-failure");
  static_assert(
      cpp17::is_same_v<
          ::elastica::collision::tt::PrimitiveTrait_t<detail::CosseratRodBlock>,
          ::elastica::collision::CapsuleFromRodPrimitive>,
      "Meta-failure");
  static_assert(
      cpp17::is_same_v< ::elastica::collision::tt::PrimitiveTrait_t<
                            detail::CosseratRodBlockSlice>,
                        ::elastica::collision::CapsuleFromRodPrimitive>,
      "Meta-failure");
  static_assert(
      cpp17::is_same_v< ::elastica::collision::tt::PrimitiveTrait_t<
                            detail::CosseratRodBlockConstSlice>,
                        ::elastica::collision::CapsuleFromRodPrimitive>,
      "Meta-failure");

  static_assert(
      cpp17::is_same_v< ::elastica::collision::tt::PrimitiveTrait_t<
                            detail::CosseratRodWithoutDamping>,
                        ::elastica::collision::CapsuleFromRodPrimitive>,
      "Meta-failure");
  static_assert(
      cpp17::is_same_v< ::elastica::collision::tt::PrimitiveTrait_t<
                            detail::CosseratRodWithoutDampingBlock>,
                        ::elastica::collision::CapsuleFromRodPrimitive>,
      "Meta-failure");
  static_assert(
      cpp17::is_same_v< ::elastica::collision::tt::PrimitiveTrait_t<
                            detail::CosseratRodWithoutDampingBlockSlice>,
                        ::elastica::collision::CapsuleFromRodPrimitive>,
      "Meta-failure");
  static_assert(
      cpp17::is_same_v< ::elastica::collision::tt::PrimitiveTrait_t<
                            detail::CosseratRodWithoutDampingBlockConstSlice>,
                        ::elastica::collision::CapsuleFromRodPrimitive>,
      "Meta-failure");

}  // namespace