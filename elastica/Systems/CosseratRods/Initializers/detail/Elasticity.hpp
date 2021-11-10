//
// Created by Tejaswin Parthasarathy on 10/29/21.
//

#pragma once

#include <tuple>

#include "Options/Options.hpp"
#include "Options/Parameter.hpp"
///
#include "Simulator/Materials.hpp"
///
#include "Systems/CosseratRods/Tags.hpp"
///
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/TMPL.hpp"
///
#include "Systems/CosseratRods/Initializers/detail/Common.hpp"
#include "Systems/CosseratRods/Initializers/detail/Transformations.hpp"

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

      class LinearHyperElasticModel {
       public:
        // the other required parameter comes from common
        using RequiredParameters = tmpl::list<>;
        using DefaultParameters = tmpl::list<>;

        template <typename OptionsTuple>
        decltype(auto) get_required_parameters(
            OptionsTuple const& options_cache) const /*noexcept*/ {
          const auto material_id =
              std::get<option_tags::Material>(options_cache).value();
          const real_t E = Material::get_youngs_modulus(material_id);
          const real_t G = Material::get_shear_modulus(material_id);
          return std::make_tuple(
              blocks::initialize<tags::BendingTwistRigidityMatrix>(
                  [E, G](std::size_t /* meta index */) {
                    return Vec3{E, E, G};
                  }),
              blocks::initialize<tags::ShearStretchRigidityMatrix>(
                  [E, G](std::size_t /* meta index */) {
                    return Vec3{G, G, E};
                  }));
        }
      };

      class LinearHyperElasticModelWithDamping
          : public LinearHyperElasticModel {
       private:
        using P = LinearHyperElasticModel;

       public:
        using ForceDampingCoefficient = option_tags::ForceDampingCoefficient;
        using TorqueDampingCoefficient = option_tags::TorqueDampingCoefficient;

        // parent has no required parameters
        using RequiredParameters =
            tmpl::list<ForceDampingCoefficient, TorqueDampingCoefficient>;
        using DefaultParameters = tmpl::list<>;

       protected:
        template <typename OptionsTuple>
        decltype(auto) get_required_parameters(
            OptionsTuple const& options_cache) const /*noexcept*/ {
          tmpl::list<ForceDampingCoefficient, TorqueDampingCoefficient> tags{};
          check_lower_bounds(options_cache, tags);

          return std::tuple_cat(
              // for things that don't need to be processed, pass as wrapped
              P::get_required_parameters(options_cache),
              pass_as_wrapped(options_cache, tags));
        }
      };

    }  // namespace detail

    struct ElasticityInitializers {
      using LinearHyperElasticModel = detail::LinearHyperElasticModel;
      using LinearHyperElasticModelWithDamping =
          detail::LinearHyperElasticModelWithDamping;
    };

  }  // namespace cosserat_rod

}  // namespace elastica