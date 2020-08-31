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
  [ ] internal_model_loads_ (n_l)
  [ ] internal_model_couples_ (tau_l)

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
  [x] shape_factor
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

  /*
   * Design:
   *
   * Ideally, we want that each and every mixin has access to the computational
   * block that has all the data members like so:
   *
   * <Traits, Block>
   * struct Geometyr, Elasticity etc.
   *
   * Then we mix them into RodPlugin (which acts like a wrapper with all the
   * mixed in classes)
   *
      template <typename CRT,                 // Cosserat Rod Traits
                typename ComputationalBlock,  // Block with implementation
                template <typename , typename >
      class... Layers>  // Skills
      class CosseratRodPlugin : public
      : public Layers<
            CRT, ComputationalBlock>...{};

      A nice feature of this desing is that each and every mixin has access
      to the block via CRTP and can access everything.

      However there is a problem with the block construction:

      First the default block template should be changed from

       // Models the block concept
        template <template <typename...> class Plugin>
        class Block;

      to

      template <template <typename Traits, typename Block ,
                    template <typename , typename >
                    class... Skills>
          class Plugin>
      class Block;

      Then during partial template specialization we have
      template <typename CRT,                  // Cosserat Rod Traits
                template <typename, typename>  // Skills
                class... Layers>
      class Block<CosseratRodPlugin<CRT, Block<CosseratRodPlugin>,
                                    Layers<CRT, Block<CosseratRodPlugin<
                                                    CRT,
   Block<CosseratRodPlugin>, Layers>
                                                      >>...>>
          : public CosseratRodPlugin<CRT, Block<CosseratRodPlugin>,
                                     Layers<CRT, Block<CosseratRodPlugin>>...>,
            private NonCopyable {
      It becomes unreadable and hard to parse whats exactly going on.


      An alternate approoch is to not template the mixins on the block, but
      to template them on the RodPlugin itself like so

   * <Traits, Plugin>
   * struct Geometyr, Elasticity etc.
   *
   * Then we can use them like so:
   *
      template <typename CRT,                 // Cosserat Rod Traits
                typename ComputationalBlock,  // Block with implementation
                template <typename , typename >
      class... Layers>  // Skills
      class CosseratRodPlugin : public
      : public Layers<
            CRT, CosseratRodPlugin<CRT, ComputaionalBlock, Layers...>>...{};

      An obvious disadvantage is that for the mixins to have access to data
      members we now need to use CRTP twice at the call site, something like

      // First self makes the mixin a CosseratRodPlugin
      // Second self makes the mixin a ComputationalBlock
      // which then has access to rod properties
      self().self().rod_properties();

      As a benefit however the instantatino is easier and muhc more readable
      The code is much more readable
  */

  /*
   * Design requirement:
   *
   * The layers should compulsarily have a:
   * - public "MemberTags" typedef
   * - public initialize function with specific signature
   */

  template <typename CRT,  // Cosserat Rod Traits
            template <typename>
            class ComputationalBlock,  // Block with implementation
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Layers>  // Skills
  class CosseratRodPlugin
      : public Layers<CRT, ComputationalBlock<CosseratRodPlugin<
                               CRT, ComputationalBlock, Layers...>>>... {
   protected:
    using Traits = CRT;
    // using Parent = detail_cosserat_rod::RodGeometry<Traits>;
    using This = CosseratRodPlugin<Traits, ComputationalBlock, Layers...>;
    using Parents = tmpl::list<Layers<CRT, This>...>;
    using Self = ComputationalBlock<This>&;
    using ConstSelf = ComputationalBlock<This> const&;

    // Import staggering tags
    using NodeTag = typename Traits::NodeTag;
    using ElementTag = typename Traits::ElementTag;
    using VoronoiTag = typename Traits::VoronoiTag;

    // Import all tag lists
    // Just transform and flatten here, although it seems weird to use
    // a "transform"
    template <typename ParentType>
    struct AddMemberTags {
      friend ParentType;
      using type = typename ParentType::MemberTags;
    };
    //    using InternalMemberTags =
    //        tmpl::flatten<tmpl::transform<Parents, AddMemberTags<tmpl::_1>>>;
    using InternalMemberTags =
        tmpl::flatten<tmpl::list<typename Layers<CRT, This>::MemberTags...>>;
    //    TypeDisplayer<InternalMemberTags> td{};
    /* using SliceTags = typename Parent::SliceTags; */

    template <typename T>
    using GetSliceType = typename Traits::template GetSliceType<T>;

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

    // Initializer always protected, initialize parent class one-by-one
    template <class InitializerTuple, typename... Members>
    void initialize(std::size_t n_elems, std::tuple<Members...>& members_,
                    InitializerTuple&& initializers) {
      EXPAND_PACK_LEFT_TO_RIGHT(
          Layers<CRT, ComputationalBlock<CosseratRodPlugin<
                          CRT, ComputationalBlock, Layers...>>>::
              initialize(n_elems, members_,
                         std::forward<InitializerTuple>(initializers)));
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

    /* Should be in respective classes
    constexpr decltype(auto) get_position() noexcept {
      return std::get<PositionCollectionTag>(self().rod_properties_);
    }

    constexpr decltype(auto) get_reference_lengths() noexcept {
      return std::get<ReferenceElementLengthCollectionTag>(
          self().rod_properties_);
    }
     */
  };

}  // namespace elastica