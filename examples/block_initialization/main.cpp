#include <iostream>
#include <tuple>

#include "Systems/CosseratRods/Block.hpp"
#include "Systems/CosseratRods/Types.hpp"
#include "Utilities/DefineTypes.h"

int main(int argc, char* argv[]) {
  using namespace elastica;
  auto position_generator =
      make_position_profile([](std::size_t i) { return 1.0 + real_t(i); });
  auto director_generator =
      make_director_profile([](std::size_t i) { return 1.0 + real_t(i); });
  auto reference_length_generator = make_reference_length_profile(
      [](std::size_t i) { return 1.0 + real_t(i); });
  auto reference_curvature_generator = make_reference_curvature_profile(
      [](std::size_t i) { return 1.0 + real_t(i); });
  auto second_position_generator =
      make_position_profile([](std::size_t i) { return 10.0 + real_t(i); });
  auto radius_generator =
      make_dimension_profile([](std::size_t i) { return i; });

  CosseratRodBlock block{};
  /*
  block.emplace_back(
      20UL, std::forward_as_tuple(position_generator, director_generator,
                                  reference_length_generator,
                                  reference_curvature_generator));

  block.emplace_back(
      10UL, std::forward_as_tuple(second_position_generator, director_generator,
                                  reference_length_generator,
                                  reference_curvature_generator));
  */
  //  block.emplace_back(
  //      20UL, std::move(position_generator), std::move(director_generator),
  //      std::move(reference_length_generator),
  //      std::move(reference_curvature_generator));

  //  block.emplace_back(20UL, make_position_profile([](std::size_t
  //  i){return 10.0 + i;}),
  //                     std::move(director_generator),
  //                     std::move(reference_length_generator),
  //                     std::move(reference_curvature_generator));

  block.emplace_back(
      make_cosserat_rod(20UL, position_generator, director_generator,
                        reference_length_generator,
                        reference_curvature_generator, radius_generator),
      make_cosserat_rod(10UL, second_position_generator, director_generator,
                        reference_length_generator,
                        reference_curvature_generator, radius_generator));

  auto& cur = block.get_lengths();
  auto& pos = block.get_reference_lengths();

  std::cout << cur << std::endl;
  std::cout << pos << std::endl;

  std::cout << block.get_force() << std::endl;
}