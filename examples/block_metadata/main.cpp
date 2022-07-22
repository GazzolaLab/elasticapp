//
// Created by Seung Hyun Kim on 10/28/21.
//

#include <fstream>
#include <iostream>
#include <sstream>

#include "Systems/Block.hpp"
#include "Systems/CosseratRods.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"

int main() {
  // Use metadata to obtain information about the variables of plugins in YAML
  // format

  // Here's a plugin provided by elastica.
  using elastica::cosserat_rod::CosseratRod;
  std::cout << blocks::metadata<CosseratRod>() << std::endl;

  // metadata also works on Block types and all types that model the Block
  // concept.
  using Block = elastica::cosserat_rod::detail::CosseratRodBlock;
  Block block{};
  std::stringstream ss;
  ss << blocks::metadata(block) << std::endl;

  // Another plugin built by the user in elastica's composition
  using Traits = elastica::cosserat_rod::DefaultCosseratRodTraits;
  using CRP = elastica::cosserat_rod::CosseratRodPlugin<
      Traits, blocks::Block,
      elastica::cosserat_rod::component::WithSquareCosseratRod,
      elastica::cosserat_rod::component::
          WithExplicitlyDampedDiagonalLinearHyperElasticModel,
      elastica::cosserat_rod::component::WithRodKinematics>;

  // Dump it directly to YAML
  {
    std::ofstream out("block_metadata.yml", std::ios_base::out);
    out << blocks::metadata<CRP>();
  }

  return 0;
}