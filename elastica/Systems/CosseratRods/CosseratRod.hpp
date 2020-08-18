#pragma once

///
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
///
#include "DataStructures/TaggedTuple.hpp"
#include "Systems/CosseratRods/Policies/Policies.hpp"

/*
 * Checklist
 *
 * Rod
  [x] masses_;
  [ ] shear_forces_;
  [ ] internal_forces_
  [ ] external_forces_;
  [ ] intrinsic_shear_strain_
  [ ] shear_strain_;
  [ ] shear_torques_;
  [ ] internal_torques_
  [ ] external_torques_;
  [ ] bending_torques_;
  [ ] rest_B
  [ ] rest_S
  [ ] state
  [x] restJ_;
  [x] rest_inv_J_;

  * Kinematics layer
  [ ] velocities_;
  [ ] omega_;
  [ ] dilatation_rate_

    tangents
      |
  positions -> lengths
               /
  Q -> curvature
  sep : rest_lengths
  sep : rset_curvatures


  Shape
  [x] radii_;
  [x] volumes_;
   |  Geometry
   |  [x] rest_lengths_
   |  [x] lengths_
   |  [x] dilatations_;
   |  [x] voronoi_rest_lengths_
   |  [x] voronoi_lengths_
   |  [x] voronoi_dilatations_
   |  [x] intrinsic_curvature_
   |  [x] curvature_;
   |  [x] tangents_;
       | GeometryBase
       | [x] positions_;
       | [x] Q_;


 */

namespace elastica {

  //  template <typename RodTraits, template <typename> class ShapePolicy,
  //            template <typename> class GrowthPolicy>
  //  class CosseratRod : public ShapePolicy<RodTraits>, GrowthPolicy<RodTraits>
  //  {
  //    using MassStorage = typename RodTraits::NodeVector;
  //
  //   private:
  //    MassStorage masses_;
  //  };

  //  template <typename ComputationalBlock, typename RodTraits,
  //            template <typename> class ShapePolicy>

  template <typename ComputationalBlock>
  class CosseratRod : public detail_cosserat_rod::RodGeometry<
                          detail_cosserat_rod::CosseratRodTraits> {
   protected:
    using Parent = detail_cosserat_rod::RodGeometry<
        detail_cosserat_rod::CosseratRodTraits>;

    using MemberTags = typename Parent::MemberTags;
    using SliceTags = typename Parent::SliceTags;

    //    using RodProperties = tmpl::as_tagged_tuple<MemberTags>;

   protected:
    constexpr inline ComputationalBlock& self() noexcept {
      return static_cast<ComputationalBlock&>(*this);
    }

    //   private:
    //    RodProperties rod_properties_;
  };

  // TODO : Blocking strategy :
  //  1. Default : free blocks
  //  2. fixed size blocks (compile time-parameter)
  template <template <typename> class A>
  class Block;

  template <>
  class Block<CosseratRod> : public CosseratRod<Block<CosseratRod>>,
                             private NonCopyable {
    using Parent = CosseratRod<Block<CosseratRod>>;
    using MemberTags = typename Parent::MemberTags;
    using SliceTags = typename Parent::SliceTags;

    using CosseratRodProperties = tmpl::as_tagged_tuple<MemberTags>;

    // No need scalable allocator
    using GhostIndices = std::vector<std::size_t>;
    using ElementSizes = std::vector<std::size_t>;

    // InitializerPacks are pack objects (a tuple) each having a bunch of generator
    // expressions to initialize a single cosserat rod
    template <typename ...InitializerPacks>
    Block(ElementSizes&& elements, std::tuple<InitializerPacks...>&& initializer_packs)
        : elements_(std::move(elements)){};
    ~Block() = default;

   private:
    // All members of CosseratRod
    CosseratRodProperties rod_properties_;
    GhostIndices ghost_indices_;
    ElementSizes elements_;
    // Extra members for book-keeping
    // 1. Indices of ghosts a std::vector<std::size_t>
    // 2. Number of elements, as a std::vector<std::size_t>
  };

}  // namespace elastica