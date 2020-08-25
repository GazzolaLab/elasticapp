#pragma once

/// Forward declarations
#include "Systems/CosseratRods/Types.hpp"
///
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
///
#include "Systems/CosseratRods/Policies/Policies.hpp"

/*
 * Checklist
 *
 * Rod
  [x] masses_;
  [x] restJ_;
  [x] rest_inv_J_;

  [ ] shear_forces_;
  [ ] external_forces_;
  [ ] intrinsic_shear_strain_
  [ ] shear_strain_;
  [ ] shear_torques_;
  [ ] external_torques_;
  [ ] bending_torques_;

  * Timestepper adapter (customization point for the Time-stepper)
   - Prerequisites : GeometryLayer, KinematicsLayer
  [ ] state

  * Elasticity layer
   - Prerequisites : GeometryLayer
  [ ] rest_B
  [ ] rest_S
  [ ] internal_forces_
  [ ] internal_torques_

  * Kinematics layer
   - Prerequisites : GeometryLayer
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
  [ ] shape_factor
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
  //   template <typename CRT,
  //      typename ComputationalBlock,
  //            template <typename, typename> class... Layers>

  template <typename ComputationalBlock>
  class CosseratRodPlugin : public detail_cosserat_rod::RodGeometry<
                                detail_cosserat_rod::CosseratRodTraits> {
   protected:
    using Traits = detail_cosserat_rod::CosseratRodTraits;
    using Parent = detail_cosserat_rod::RodGeometry<Traits>;
    using This = CosseratRodPlugin<ComputationalBlock>;
    using Self = ComputationalBlock&;
    using ConstSelf = ComputationalBlock const&;

    // Import staggering tags
    using NodeTag = typename Traits::NodeTag;
    using ElementTag = typename Traits::ElementTag;
    using VoronoiTag = typename Traits::VoronoiTag;

    // Import all tag lists
    using InternalMemberTags = typename Parent::MemberTags;
    /* using SliceTags = typename Parent::SliceTags; */

    template <typename T>
    using GetSliceType = Traits::template GetSliceType<T>;
    using NodeMemberTags =
        tmpl::filter<InternalMemberTags,
                     typename Traits::template IsNodeStaggered<tmpl::_1>>;
    using ElementMemberTags =
        tmpl::filter<InternalMemberTags,
                     typename Traits::template IsElementStaggered<tmpl::_1>>;
    using VoronoiMemberTags =
        tmpl::filter<InternalMemberTags,
                     typename Traits::template IsVoronoiStaggered<tmpl::_1>>;

    using MemberTags =
        tmpl::append<NodeMemberTags, ElementMemberTags, VoronoiMemberTags>;
    /* using SliceTags = typename Parent::SliceTags; */
    using SliceTags =
        tmpl::transform<MemberTags, tmpl::bind<GetSliceType, tmpl::_1>>;

   protected:
    //    using Parent::print;
    inline constexpr Self self() noexcept { return static_cast<Self>(*this); }
    inline constexpr ConstSelf self() const noexcept {
      return static_cast<ConstSelf>(*this);
    }

   protected:
    template <typename StaggeredTag>
    inline constexpr auto get_dofs() const noexcept {
      return StaggeredTag::get_dofs(self().n_elements_);
    }

   public:
    // Helpers
    inline constexpr auto n_elements() const noexcept {
      return get_dofs<ElementTag>();
    }

    inline constexpr auto n_nodes() const noexcept {
      return get_dofs<NodeTag>();
    }

    inline constexpr auto n_voronois() const noexcept {
      return get_dofs<VoronoiTag>();
    }

    constexpr decltype(auto) get_position() noexcept {
      return std::get<PositionCollectionTag>(self().rod_properties_);
    }

    constexpr decltype(auto) get_reference_lengths() noexcept {
      return std::get<ReferenceElementLengthCollectionTag>(
          self().rod_properties_);
    }
  };

}  // namespace elastica