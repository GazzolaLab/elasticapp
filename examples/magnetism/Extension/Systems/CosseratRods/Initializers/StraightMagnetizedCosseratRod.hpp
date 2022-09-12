#pragma once

#include <elastica/Systems/CosseratRods/Initializers/StraightCosseratRod.hpp>
#include <string>

#include "Extension/Systems/CosseratRods/Initializers/detail/Magnetism.hpp"

namespace elastica {

  namespace cosserat_rod {

    class StraightUniformMagnetizedCosseratRodInitializer
        : public ComposeInitializers<
              Initializers::CosseratRod,                                     //
              Initializers::Geometry::StraightRod,                           //
              Initializers::Elasticity::LinearHyperElasticModel,  //
              Initializers::Kinematics::DefaultRodKinematics,
              MagnetismInitializers::UniformMagnetizedRod> {
     public:
      using P = ComposeInitializers<
          Initializers::CosseratRod,                                     //
          Initializers::Geometry::StraightRod,                           //
          Initializers::Elasticity::LinearHyperElasticModel,  //
          Initializers::Kinematics::DefaultRodKinematics,
          MagnetismInitializers::UniformMagnetizedRod>;
      using P::P;

     public:
      using options = RequiredParameters;
    };

    class StraightNonUniformMagnetizedCosseratRodInitializer
        : public ComposeInitializers<
              Initializers::CosseratRod,                                     //
              Initializers::Geometry::StraightRod,                           //
              Initializers::Elasticity::LinearHyperElasticModel,  //
              Initializers::Kinematics::DefaultRodKinematics,
              MagnetismInitializers::NonUniformMagnetizedRod> {
     public:
      using P = ComposeInitializers<
          Initializers::CosseratRod,                                     //
          Initializers::Geometry::StraightRod,                           //
          Initializers::Elasticity::LinearHyperElasticModel,  //
          Initializers::Kinematics::DefaultRodKinematics,
          MagnetismInitializers::NonUniformMagnetizedRod>;
      using P::P;

     public:
      using options = RequiredParameters;
    };

  }  // namespace cosserat_rod

}  // namespace elastica