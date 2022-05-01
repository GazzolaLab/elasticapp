//
// Created by Tejaswin Parthasarathy on 3/24/22.
//

#include "Simulator/Environments/Primitives/ImposedPrimitiveAdapter.hpp"  // need definition for type-trait to work
#include "Simulator/Environments/Primitives/SpherePrimitive.hpp"
#include "Simulator/Environments/Primitives/TypeTraits.hpp"

namespace {

  /// [is_rigid_primitive_eg]
  struct A {};
  static_assert(not ::elastica::collision::tt::IsRigidPrimitive<A>::value,
                "Meta failure!");
  static_assert(::elastica::collision::tt::IsRigidPrimitive_v<
                    ::elastica::collision::SpherePrimitive>,
                "Meta failure!");
  /// [is_rigid_primitive_eg]

  /// [primitive_trait_eg]
  using ::elastica::collision::ImposedPrimitiveAdapter;
  using ::elastica::collision::SpherePrimitive;
  using ::elastica::collision::tt::PrimitiveTrait;
  using ::elastica::collision::tt::PrimitiveTrait_t;
  static_assert(cpp17::is_same_v<typename PrimitiveTrait<SpherePrimitive>::type,
                                 SpherePrimitive>,
                "Meta failure!");
  static_assert(
      cpp17::is_same_v<PrimitiveTrait_t<SpherePrimitive>, SpherePrimitive>,
      "Meta failure!");

  using ImposedSphere = ImposedPrimitiveAdapter<SpherePrimitive>;
  static_assert(
      cpp17::is_same_v<PrimitiveTrait_t<ImposedSphere>, SpherePrimitive>,
      "Meta failure!");
  /// [primitive_trait_eg]

}  // namespace

/// [primitive_trait_custom_eg]
struct C {};  // Will not customize
struct D {};  // Will be customized

namespace elastica {

  namespace customization {

    template <>
    struct PrimitiveTrait<D> {
      using type = int;
    };

  }  // namespace customization

}  // namespace elastica

// Doesn't compile!
// static_assert(cpp17::is_same_v<PrimitiveTrait_t<C>, int>, "Meta failure!");
static_assert(cpp17::is_same_v<PrimitiveTrait_t<D>, int>, "Meta failure!");
/// [primitive_trait_custom_eg]
