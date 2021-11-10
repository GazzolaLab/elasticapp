//
// Created by Seung Hyun Kim on 10/28/21.
//

#include "Systems/Block.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"

int main (){
  using Traits = elastica::cosserat_rod::DefaultCosseratRodTraits;
  using CRP = elastica::cosserat_rod::CosseratRodPlugin<
      Traits,
      blocks::Block,
      elastica::cosserat_rod::component::WithCircularCosseratRod,
      elastica::cosserat_rod::component::WithExplicitlyDampedDiagonalLinearHyperElasticModel,
      elastica::cosserat_rod::component::WithRodKinematics>;
  using Block = blocks::Block<CRP>;

  blocks::display_variable_summary<CRP>();

  return 0;
}