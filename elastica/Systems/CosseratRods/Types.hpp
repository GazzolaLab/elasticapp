#pragma once

#include "Systems/Block/Block.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Policies/StrongTypes.hpp"
#include "Systems/CosseratRods/Policies/Types.hpp"
#include "Utilities/DefineTypes.h"

namespace elastica {

  //////////////////////////////////////////////////////////////////////////////
  // Namespace promotion
  //////////////////////////////////////////////////////////////////////////////
  using detail_cosserat_rod::make_dimension_profile;
  using detail_cosserat_rod::make_director_profile;
  using detail_cosserat_rod::make_position_profile;
  using detail_cosserat_rod::make_reference_curvature_profile;
  using detail_cosserat_rod::make_reference_length_profile;

  using detail_cosserat_rod::CircleCrossSection;
  using detail_cosserat_rod::LinearHyperelasticModel;
  using detail_cosserat_rod::SquareCrossSection;

  //////////////////////////////////////////////////////////////////////////////
  // Forward declarations of cosserat rod types
  //////////////////////////////////////////////////////////////////////////////

  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename, typename> class... Layers>
  class CosseratRodPlugin;

  // Can instantiate it in a CPP file
  using CosseratRodBlock =
      Block<CosseratRodPlugin<detail_cosserat_rod::CosseratRodTraits, Block,
                              CircleCrossSection, LinearHyperelasticModel>>;

  template <typename...>
  class CosseratBlockInitializer;

  //  class RigidBodyCollection;
  //  class SphereCollection;
}  // namespace elastica
