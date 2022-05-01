//
// Created by Tejaswin Parthasarathy on 4/20/22.
//

#include "Simulator/Environments/Primitives/RigidBodyPrimitive.hpp"
#include "Simulator/Environments/Transfer/TypeTraits.hpp"

namespace {

  /// [IsSystemDefaultTransferable_eg]
  struct DefaultTransferableSystem {
    std::size_t get_dimensions(std::size_t) const { return 0UL; }
    int get_position(std::size_t) const { return 0; }
    int get_director(std::size_t) const { return 0; }
    int get_velocity(std::size_t) const { return 0; }
    int get_angular_velocity(std::size_t) const { return 0; }
    int get_force(std::size_t) const { return 0; }
    int get_torque(std::size_t) const { return 0; }
  };

  struct NonDefaultTransferableSystem {
    // implements only some methods
    int get_director(std::size_t) const { return 0; }
    int get_velocity(std::size_t) const { return 0; }
  };

  static_assert(elastica::collision::tt::IsSystemDefaultTransferable<
                    DefaultTransferableSystem>::value,
                "Meta-failure!");
  static_assert(not elastica::collision::tt::IsSystemDefaultTransferable<
                    NonDefaultTransferableSystem>::value,
                "Meta-failure!");
  /// [IsSystemDefaultTransferable_eg]

  /// [IsPrimitiveDefaultTransferable_eg]
  struct DefaultTransferablePrimitive
      : public elastica::collision::RigidBodyPrimitive {
    void transfer(){};
  };
  struct NonDefaultTransferablePrimitive {};
  static_assert(elastica::collision::tt::IsPrimitiveDefaultTransferable<
                    DefaultTransferablePrimitive>::value,
                "Meta-failure!");
  static_assert(not elastica::collision::tt::IsPrimitiveDefaultTransferable<
                    NonDefaultTransferablePrimitive>::value,
                "Meta-failure!");
  /// [IsPrimitiveDefaultTransferable_eg]
}  // namespace

/// [IsDefaultTransferable_eg]
namespace elastica {

  namespace customization {

    template <>
    struct PrimitiveTrait<DefaultTransferableSystem> {
      using type = DefaultTransferablePrimitive;
    };

    template <>
    struct PrimitiveTrait<NonDefaultTransferableSystem> {
      // even if primitive is default transferable, system is still not
      using type = DefaultTransferablePrimitive;
    };
  }  // namespace customization

}  // namespace elastica

static_assert(elastica::collision::tt::IsDefaultTransferable<
                  DefaultTransferableSystem>::value,
              "Meta-failure!");
static_assert(not elastica::collision::tt::IsDefaultTransferable<
                  NonDefaultTransferableSystem>::value,
              "Meta-failure!");
/// [IsDefaultTransferable_eg]
