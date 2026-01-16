//
// Created by Tejaswin Parthasarathy on 6/16/21.
//

#include "Systems/CosseratRods/Specialize.hpp"

namespace {

  /// [specialize_eg]
  // some mock structures
  struct CosseratTraits;
  template <typename Plugin>
  struct DummyBlock;
  template <typename Trait, typename Block>
  struct DummyComponent;

  using MyCosseratRod =
      elastica::cosserat_rod::CosseratRodPlugin<CosseratTraits, DummyBlock,
                                                DummyComponent>;
  struct SnakeTag;
  using Snake = elastica::cosserat_rod::specialize<MyCosseratRod, SnakeTag>;
  // Snake is now a special MyCosseratRod! Its an extended type.

  // you can further specialize it if need arises
  struct SmolSnakeTag;
  using SmolSnake = elastica::cosserat_rod::specialize<Snake, SmolSnakeTag>;
  /// [specialize_eg]

  static_assert(
      std::is_same_v<
          Snake, elastica::cosserat_rod::TaggedCosseratRodPlugin<
                     CosseratTraits, DummyBlock, SnakeTag, DummyComponent>>,
      "Specialize meta-failure!");
  static_assert(
      std::is_same_v<
          SmolSnake,
          elastica::cosserat_rod::TaggedCosseratRodPlugin<
              CosseratTraits, DummyBlock, SmolSnakeTag, DummyComponent>>,
      "Specialize meta-failure!");

}  // namespace