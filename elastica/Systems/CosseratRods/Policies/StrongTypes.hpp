#pragma once

#include "Utilities/NamedType.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

// Note that for new types, we can template it, but then we lose type deduction
#define xxx(x, y) x##y
#define CONCAT(x, y) xxx(x, y)
#define MAKE_PROFILE_TYPES(M, m)                                      \
  namespace tags {                                                    \
    struct CONCAT(M, ProfileTag) {};                                  \
                                                                      \
    namespace detail {                                                \
      template <typename Func>                                        \
      using CONCAT(M, ProfileTagInternal) =                           \
          utilities::NamedType<Func, CONCAT(M, ProfileTag)>;          \
    }                                                                 \
  }                                                                   \
                                                                      \
  template <typename Func>                                            \
  constexpr tags::detail::CONCAT(M, ProfileTagInternal)<Func> CONCAT( \
      make_, CONCAT(m, _profile))(Func && value) {                    \
    return tags::detail::CONCAT(                                      \
        M, ProfileTagInternal)<Func>(std::forward<Func>(value));      \
  }

    /* This macro generates a strong type and a make_function
     * For example
     * MAKE_PROFILE_TYPES(Radius, radius);
     * results in
     *
        template <typename Func>
        using RadiusProfile = utilities::NamedType<Func, struct
     RadiusProfileTag>;

      template <typename Func>
      constexpr RadiusProfile<Func> make_radius_profile(Func&& value) {
        return RadiusProfile<Func>(std::forward<Func>(value));
      }
     *
     * */
    MAKE_PROFILE_TYPES(Position, position);
    MAKE_PROFILE_TYPES(Director, director);
    MAKE_PROFILE_TYPES(ReferenceLength, reference_length);
    MAKE_PROFILE_TYPES(ReferenceCurvature, reference_curvature);
    // Stand in for radius / square edge
    MAKE_PROFILE_TYPES(Dimension, dimension);
    MAKE_PROFILE_TYPES(Density, density);
    MAKE_PROFILE_TYPES(Velocity, velocity);
    MAKE_PROFILE_TYPES(Omega, omega);
    MAKE_PROFILE_TYPES(BendTwistRigidity, bend_twist_rigidity);
    MAKE_PROFILE_TYPES(ShearStretchRigidity, shear_stretch_rigidity);
    MAKE_PROFILE_TYPES(ForceDamping, force_damping);
    MAKE_PROFILE_TYPES(TorqueDamping, torque_damping);
#undef MAKE_PROFILE_TYPES
#undef CONCAT
#undef xxx

  }  // namespace detail_cosserat_rod

}  // namespace elastica