#pragma once

#include "Systems/CosseratRods/Policies/StrongTypes.hpp"
#include "Systems/Block/Block.hpp"

namespace elastica {

  //////////////////////////////////////////////////////////////////////////////
  // Forward declarations of cosserat rod types
  //////////////////////////////////////////////////////////////////////////////

  using detail_cosserat_rod::make_director_profile;
  using detail_cosserat_rod::make_position_profile;
  using detail_cosserat_rod::make_reference_curvature_profile;
  using detail_cosserat_rod::make_reference_length_profile;

  template <typename ComputationalBlock>
  class CosseratRodPlugin;

  using CosseratRodBlock = Block<CosseratRodPlugin>;
  template <typename ...>
  class CosseratBlockInitializer;

  //  class RigidBodyCollection;
  //  class SphereCollection;
}  // namespace elastica
