#pragma once

#include <cstddef>

#include "Systems/CosseratRods/Policies/GeometryPolicies/Types.hpp"
#include "Utilities/DefineTypes.h"

namespace elastica {

  namespace detail_cosserat_rod {

    template <typename RodTraits>
    struct RodKinematics {
     protected:
      using Self = RodKinematics<RodTraits>;
      using Traits = RodTraits;
      using Velocities = typename Traits::NodeMatrix;
      using AngularVelocities = typename Traits::ElementMatrix;
      using Dilatations = typename Traits::ElementVector;

      // Should take in a polymorphic allocator as argument and
      // only allocate memory from that resource
      template <typename VelGen, typename OmegaGen>
      RodKinematics(std::size_t n_elems,
                    VelocityProfile<VelGen>&& velocity_profile,
                    OmegaProfile<OmegaGen>&& omega_profile)
          : velocities_(Traits::dimensions(), Traits::n_nodes(n_elems)),
            omegas_(Traits::dimensions(), n_elems),
            // Leaving this uninitialized for now as it requires
            // position information
            dilatation_rates_(n_elems, real_t(0.0)) {
        {
          auto& f = velocity_profile.get();
          for (std::size_t i = 0; i < Traits::n_nodes(n_elems); ++i) {
            velocities_[i] = f(i);
          }
        }

        {
          auto& f = omega_profile.get();
          for (std::size_t i = 0; i < n_elems; ++i) {
            omegas_[i] = f(i);
          }
        }
      }

     protected:
      // TODO : Think about aligning them together
      Velocities velocities_;
      AngularVelocities omegas_;
      Dilatations dilatation_rates_;
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
