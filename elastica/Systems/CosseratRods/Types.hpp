#pragma once

#include "Systems/Block/Block.hpp"
#include "Systems/Block/BlockSlice.hpp"
//#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Policies/StrongTypes.hpp"
#include "Systems/CosseratRods/Policies/Types.hpp"
#include "Utilities/DefineTypes.h"

namespace elastica {

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  // Namespace promotion
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  using detail_cosserat_rod::make_bend_twist_rigidity_profile;
  using detail_cosserat_rod::make_dimension_profile;
  using detail_cosserat_rod::make_director_profile;
  using detail_cosserat_rod::make_force_damping_profile;
  using detail_cosserat_rod::make_position_profile;
  using detail_cosserat_rod::make_reference_curvature_profile;
  using detail_cosserat_rod::make_reference_length_profile;
  using detail_cosserat_rod::make_shear_stretch_rigidity_profile;
  using detail_cosserat_rod::make_torque_damping_profile;

  // Cross sections
  using detail_cosserat_rod::CircleCrossSection;
  using detail_cosserat_rod::SquareCrossSection;

  // Elastic models
  using detail_cosserat_rod::DampedLinearHyperelasticModel;
  using detail_cosserat_rod::LinearHyperelasticModel;

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  // Forward declarations of cosserat rod types
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  namespace detail_cosserat_rod {

    struct CosseratRodTraits;

  }

  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename, typename> class... Layers>
  class CosseratRodPlugin;

  // Declare a specialization of the CosseratBlockSlice ckass;
  template <typename CRT,                  // Cosserat Rod Traits
            template <typename, typename>  // Skills
            class... Layers>
  class BlockSlice<CosseratRodPlugin<CRT, BlockSlice, Layers...>>;

  namespace slices {

    template <typename CRT,                  // Cosserat Rod Traits
              template <typename, typename>  // Skills
              class... Layers>
    constexpr decltype(auto) get(
        BlockSlice<CosseratRodPlugin<CRT, BlockSlice, Layers...>>&);

    template <typename CRT,                  // Cosserat Rod Traits
              template <typename, typename>  // Skills
              class... Layers>
    constexpr decltype(auto) get(
        BlockSlice<CosseratRodPlugin<CRT, BlockSlice, Layers...>> const&);

  }  // namespace slices

  // Can instantiate it in a CPP file
  using CosseratRodBlock =
      Block<CosseratRodPlugin<detail_cosserat_rod::CosseratRodTraits, Block,
                              CircleCrossSection, LinearHyperelasticModel>>;

  using DampedCosseratRodBlock = Block<
      CosseratRodPlugin<detail_cosserat_rod::CosseratRodTraits, Block,
                        CircleCrossSection, DampedLinearHyperelasticModel>>;

  template <typename /* Cosserat Rod Traits */>
  struct CosseratBlockInitializer;

  //  class RigidBodyCollection;
  //  class SphereCollection;
}  // namespace elastica
