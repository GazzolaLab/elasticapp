//
// Created by Tejaswin Parthasarathy on 10/29/21.
//

#pragma once

#include "Options/Options.hpp"
#include "Options/Parameter.hpp"
#include "Systems/CosseratRods/Tags.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      namespace option_tags {

        // can be used for torque damping as well
        struct ForceDampingCoefficient : Options::Parameter<real_t> {
          using P = Options::Parameter<real_t>;
          using P::P;
          static constexpr Options::String help = {
              "Velocity and angular velocity damping coefficient"};
          static type lower_bound() noexcept { return 0.0; }
          using TagType = tags::ForceDamping;
        };

        // can be used for torque damping as well
        struct TorqueDampingCoefficient : Options::Parameter<real_t> {
          using P = Options::Parameter<real_t>;
          using P::P;
          static constexpr Options::String help = {
              "Velocity and angular velocity damping coefficient"};
          static type lower_bound() noexcept { return 0.0; }
          using TagType = tags::TorqueDamping;
        };

      }  // namespace option_tags

    }  // namespace detail

    struct ElasticityInitializers {
      using LinearHyperElasticModel = int;
    };

  }  // namespace cosserat_rod

}  // namespace elastica