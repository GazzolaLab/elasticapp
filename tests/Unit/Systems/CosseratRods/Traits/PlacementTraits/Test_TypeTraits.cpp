//
// Created by Tejaswin Parthasarathy on 5/14/21.
//

#include "Systems/CosseratRods/Traits/PlacementTrait.hpp"
#include "Systems/CosseratRods/Traits/PlacementTraits/TypeTraits.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace {
  using namespace elastica::cosserat_rod;

  /// [type_trait_def]
  using TypeTrait =
      elastica::cosserat_rod::tt::PlacementTypeTrait<PlacementTrait>;
  /// [type_trait_def]

  /// [on_node_eg]
  struct VariableOnNode : public PlacementTrait::OnNode {};
  struct VariableNotOnNode : public PlacementTrait::OnElement {};

  static_assert(TypeTrait::IsOnNode<VariableOnNode>::value,
                "Placement failure!");
  static_assert(not TypeTrait::IsOnNode<VariableNotOnNode>::value,
                "Placement failure!");
  /// [on_node_eg]

  /// [on_element_eg]
  struct VariableOnElement : public PlacementTrait::OnElement {};
  struct VariableNotOnElement : public PlacementTrait::OnNode {};

  static_assert(TypeTrait::IsOnElement<VariableOnElement>::value,
                "Placement failure!");
  static_assert(not TypeTrait::IsOnElement<VariableNotOnElement>::value,
                "Placement failure!");
  /// [on_element_eg]

  /// [on_voronoi_eg]
  struct VariableOnVoronoi : public PlacementTrait::OnVoronoi {};
  struct VariableNotOnVoronoi : public PlacementTrait::OnNode {};

  static_assert(TypeTrait::IsOnVoronoi<VariableOnVoronoi>::value,
                "Placement failure!");
  static_assert(not TypeTrait::IsOnVoronoi<VariableNotOnVoronoi>::value,
                "Placement failure!");
  /// [on_voronoi_eg]

  /// [on_rod_eg]
  struct VariableOnRod : public PlacementTrait::OnRod {};
  struct VariableNotOnRod : public PlacementTrait::OnNode {};

  static_assert(TypeTrait::IsOnRod<VariableOnRod>::value, "Placement failure!");
  static_assert(not TypeTrait::IsOnRod<VariableNotOnRod>::value,
                "Placement failure!");
  /// [on_rod_eg]

}  // namespace