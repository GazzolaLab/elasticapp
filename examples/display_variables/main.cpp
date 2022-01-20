//
// Created by Seung Hyun Kim on 10/28/21.
//

#include "Systems/Block.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"

int main (){
  // A plugin provided by elastica
  using elastica::cosserat_rod::CosseratRod;
  blocks::display_variable_summary<CosseratRod>();


  // A plugin built by the user elastica
  using Traits = elastica::cosserat_rod::DefaultCosseratRodTraits;
  using CRP = elastica::cosserat_rod::CosseratRodPlugin<
      Traits,
      blocks::Block,
      elastica::cosserat_rod::component::WithSquareCosseratRod,
      elastica::cosserat_rod::component::WithExplicitlyDampedDiagonalLinearHyperElasticModel,
      elastica::cosserat_rod::component::WithRodKinematics>;

  blocks::display_variable_summary<CRP>();

  return 0;
}