#pragma once

#include "Utilities/NamedType.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

// Note that for new types, we can template it, but then we lose type deduction
#define xxx(x, y) x##y
#define CONCAT(x, y) xxx(x, y)
#define MAKE_PROFILE_TYPES(M, m)                                      \
  namespace tags {                                                    \
    template <typename Func>                                          \
    using CONCAT(M, Profile) =                                        \
        utilities::NamedType<Func, struct CONCAT(M, ProfileTag)>;     \
  }                                                                   \
                                                                      \
  template <typename Func>                                            \
  constexpr tags::CONCAT(M, Profile)<Func> CONCAT(                    \
      make, CONCAT(m, _profile))(Func && value) {                     \
    return tags::CONCAT(M, Profile)<Func>(std::forward<Func>(value)); \
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
    MAKE_PROFILE_TYPES(Radius, radius);
    MAKE_PROFILE_TYPES(Density, density);
    MAKE_PROFILE_TYPES(Velocity, velocity);
    MAKE_PROFILE_TYPES(Omega, omega);
#undef STAMP_OUT_SIMULATION_METHOD_CHOOSERS
#undef CONCAT
#undef xxx

  }  // namespace detail_cosserat_rod

}  // namespace elastica