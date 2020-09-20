#pragma once
#include <cmath>
#include <utility>

///// Types always first
#include "Systems/CosseratRods/Policies/StrongTypes.hpp"
#include "Utilities/DefineTypes.h"
/////
#include "Systems/CosseratRods/CosseratRodTraitMacros.hpp"
#include "Systems/CosseratRods/Policies/GeometryPolicies/RodGeometry.hpp"
#include "Systems/CosseratRods/TypeTraits/Tags.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {
  //  public RodKinematics<RodTraits>
  // Sparse_strategy
  // Dense strategy
  namespace detail_cosserat_rod {

    /* Lessons learnt:
     * We wanted to CRTP multiple times to achieve multiple-inheritance
     * during compile-time. However putting the tag structs inside the
     * CRTP base class is problematic as the tags are also stamped with
     * the CRTP-ed derived class (aka the CrossSection template) below.
     * For some reason (still unknown), this causes the tuples to not find
     * the tags initialized from the block ||-> plugin || -> circle geometry
     * which is then specially CRTped to the CrossSectionBase class.
     *
     * The error comes because
     * We search for
        elastica::detail_cosserat_rod::CosseratRodTraits::VectorSlice<elastica::detail_cosserat_rod::CrossSectionBase<elastica::detail_cosserat_rod::CosseratRodTraits,
        elastica::detail_cosserat_rod::CircleCrossSection<elastica::detail_cosserat_rod::CosseratRodTraits,
        elastica::Block<elastica::CosseratRodPlugin<elastica::detail_cosserat_rod::CosseratRodTraits,
        elastica::Block, elastica::detail_cosserat_rod::CircleCrossSection> > >
        >::VolumeCollectionTag, elastica::detail_cosserat_rod::ElementTagImpl>;

        But in the tuple we only have
        elastica::detail_cosserat_rod::CosseratRodTraits::VectorSlice<elastica::detail_cosserat_rod::CrossSectionBase<elastica::detail_cosserat_rod::CosseratRodTraits,
        elastica::detail_cosserat_rod::CircleCrossSection<elastica::detail_cosserat_rod::CosseratRodTraits,
        elastica::CosseratRodPlugin<elastica::detail_cosserat_rod::CosseratRodTraits,
        elastica::Block, elastica::detail_cosserat_rod::CircleCrossSection> >
        >::VolumeCollectionTag, elastica::detail_cosserat_rod::ElementTagImpl>,

        Notice how an extra "Block" template gets added to the search parameter
       around the CosseratRodPlugin. This seems to be the right type to be
       searched for, at least while looking at the hierarchy that we have.
     However in the tuple types there's a deficiency of the block types for some
     reason.

       To avoid these hassle, we again split the CrossSectionBase into
     CrossSectionTraits templated on the CosseratRodTraits. The sole purpose of
     this parent is just to provide meta-data on what the cross-section base
     should contain.
     *
     */

    /*!
     *
     * @tparam CRT
     */
    template <typename CRT>
    class CrossSectionTraits {
     private:
      using Traits = CRT;

     protected:
      /*
       * Generative Tags : To be filled in by generators
       */
      ELASTICA_BLOCK_REGISTER_INITIALIZED_TYPE(DimensionCollectionTag,
                                               ElementTag, VectorTag,
                                               tags::DimensionProfileTag);

      /*
       * Computed Tags : Can be filled in
       */
      ELASTICA_BLOCK_REGISTER_COMPUTED_TYPE(VolumeCollectionTag, ElementTag,
                                            VectorTag);

     protected:
      using MemberTags =
          tmpl::list<DimensionCollectionTag, VolumeCollectionTag>;
    };

    /*
     * In this multiple CRTP inheritance, another trick is used. We know
     * that the CrossSection class will only have static members. Then
     * instead of templating on the ComputaionalBlock (the impl) class
     * we template on the CrossSection instead and give it a friend access.
     * Thie leads to a more "sensible" code since only CrossSection can
     * be used as the final class (aka CrossSectionBase cannot be used
     * to instantiate a Cosserat Rod), thus preventing wrong misuse. Also
     * the CrossSection hierarchy is self contained and can be reused
     * without worrying about blocks getting interchanged in the CrossSection
     * Base and CrossSection (since only one block is templated)
     *
     *
     */
    template <typename CRT,           // Cosserat Rod Traits
              typename CrossSection>  // CS
    class CrossSectionBase : public RodGeometry<CRT>,
                             public CrossSectionTraits<CRT> {
     protected:
      using Traits = CRT;
      using This = CrossSectionBase<Traits, CrossSection>;
      using GeometryParent = RodGeometry<Traits>;
      using CST = CrossSectionTraits<Traits>;
      //      using KinematicsParent = RodKinematics<RodTraits>;

     private:
      using ThisMemberTags = typename CST::MemberTags;

      using GeneratedTags =
          tmpl::filter<ThisMemberTags,
                       tt::CheckForInitializerTagType<tmpl::_1>>;
      using ComputedTags = tmpl::list_difference<ThisMemberTags, GeneratedTags>;

      using ParentMemberTags = typename GeometryParent::MemberTags;

      // NOT POSSIBLE
      //  No easy syntax to say that I'm using all structs defined in Parent::
      //  MemberTags?
      //      using Parent::std::tuple_element<1UL,
      //      tmpl::as_tuple<ParentMemberTags>>::type;

      // internal
      using Self = CrossSection;
      using Reference = Self&;
      using ConstReference = Self const&;
      inline constexpr Reference self() noexcept {
        return static_cast<Reference>(*this);
      }
      inline constexpr ConstReference self() const noexcept {
        return static_cast<ConstReference>(*this);
      }

      template <typename PropertyTag>
      inline constexpr decltype(auto) get() noexcept{return self().template get<PropertyTag>();}

      template <typename PropertyTag>
      inline constexpr decltype(auto) get() const noexcept{return self().template get<PropertyTag>();}

     protected:
      using MemberTags = tmpl::append<ParentMemberTags, ThisMemberTags>;

      using typename CST::DimensionCollectionTag;
      using typename CST::VolumeCollectionTag;

     protected:
      // Initializer always protected
      template <typename CosseratSlice, typename CosseratInitializer>
      void initialize(CosseratSlice& property_slices,
                      CosseratInitializer&& initializers) {
        // 1. Initialize parent
        GeometryParent::initialize(
            property_slices, std::forward<CosseratInitializer>(initializers));

        // 2. Initialize generators first
        struct {
          CosseratInitializer value;
        } cap{std::forward<CosseratInitializer>(initializers)};
        tmpl::for_each<This::GeneratedTags>(
            [&property_slices,
             cap = std::move(cap)](auto generated_member_tag) mutable -> void {
              using GeneratedMemberTag =
                  tmpl::type_from<decltype(generated_member_tag)>;

              auto& member(property_slices.template get<GeneratedMemberTag>());
              // TODO : proper move semantics
              auto&& initializer(cap.value.template get<GeneratedMemberTag>());

              auto dofs = GeneratedMemberTag::get_dofs(cap.value.n_elems);
              for (std::size_t i = 0; i < dofs; ++i) {
                // TODO : This doesn't work on slices, consider having the
                // slice member in the initializer itself
                GeneratedMemberTag::slice(member, i) = initializer(i);
              }
            });

        // 3. Computed tags

        {
          auto& volumes(property_slices.template get<VolumeCollectionTag>());
          auto& dimensions(
              property_slices.template get<DimensionCollectionTag>());
          auto& lengths(property_slices.template get<
                        typename GeometryParent::ElementLengthCollectionTag>());

          volumes = dimensions * lengths;
        }
      }

     public:
      using float_type = real_t;

      // DONE : Should really be an enum class to prevent clashes with lab frame
      // from domain types X, Y, Z
      // Some static typedefs for ease of use in the Cosserat Rod class
      //**Type definitions******************************************************
      //! The type of director ID
      enum class DirectorType : std::uint8_t { d1, d2, d3 };
      //************************************************************************
      static constexpr DirectorType D1 = DirectorType::d1;
      static constexpr DirectorType D2 = DirectorType::d2;
      static constexpr DirectorType D3 = DirectorType::d3;

      // These are all interface functions
      inline constexpr auto get_dimension() const noexcept {
        return This::get<DimensionCollectionTag>();
      }

      inline constexpr decltype(auto) get_dimension() noexcept {
        return This::get<DimensionCollectionTag>();
      }

      inline constexpr auto get_dimension(std::size_t i) const noexcept
          -> float_type {
        return get_dimension()[i];
      }

      inline constexpr decltype(auto) get_volumes() noexcept {
        return This::get<VolumeCollectionTag>();
      }

      inline constexpr auto get_volumes(std::size_t i) const noexcept
          -> float_type {
        return This::get<VolumeCollectionTag>()[i];
      }

      inline constexpr auto get_area() const noexcept {
        // Should give out a VecVecExpr rather than Dimensions object
        // for expression template mechanisms
        return self().area_impl(This::get_dimension());
      }

      inline constexpr auto get_area(std::size_t i) const noexcept
          -> float_type {
        return self().area_impl(This::get_dimension(i));
      }

      template <DirectorType Dim, Requires<(Dim < This::D3)> = nullptr>
      inline constexpr auto get_second_moment_of_area(
          std::size_t i) const noexcept -> float_type {
        return self().second_moment_of_area_impl(This::get_dimension(i));
      }

      template <DirectorType Dim, Requires<(Dim == This::D3)> = nullptr>
      inline constexpr auto get_second_moment_of_area(
          std::size_t i) const noexcept -> float_type {
        // Perpendicular axis theorem
        return float_type(2.0) *
               self().second_moment_of_area_impl(This::get_dimension(i));
      }

      template <DirectorType Dim, Requires<(Dim < This::D3)> = nullptr>
      inline constexpr auto get_second_moment_of_area() const noexcept {
        // Should give out a VecVecExpr rather than Dimensions object
        // for expression template mechanisms
        return self().second_moment_of_area_impl(This::get_dimension());
      }

      template <DirectorType Dim, Requires<(Dim == This::D3)> = nullptr>
      inline constexpr auto get_second_moment_of_area() const noexcept {
        // Should give out a VecVecExpr rather than Dimensions object
        // for expression template mechanisms
        return real_t(2.0) *
               self().second_moment_of_area_impl(This::get_dimension());
      }

//      inline constexpr auto get_second_moment_of_area() const noexcept {
//        return Vec3{get_second_moment_of_area<D1>(),
//                    get_second_moment_of_area<D2>(),
//                    get_second_moment_of_area<D3>()};
//      }
//
      inline constexpr auto get_second_moment_of_area(
          std::size_t i) const noexcept {
        return Vec3{get_second_moment_of_area<D1>(i),
                    get_second_moment_of_area<D2>(i),
                    get_second_moment_of_area<D3>(i)};
      }

      inline constexpr decltype(auto) get_lengths() noexcept {
        return This::get<typename GeometryParent::ElementLengthCollectionTag>();
      }

      inline constexpr decltype(auto) get_reference_lengths() noexcept {
        return This::get<typename GeometryParent::ReferenceElementLengthCollectionTag>();
      }

      inline constexpr decltype(auto) get_directors() noexcept {
        return This::get<typename GeometryParent::DirectorCollectionTag>();
      }

      // Technically these can be put in an interface class that has
      // the ability to compute curvature/shear etc., but we can leave
      // it aside for now.

     private:
      // Translational functions
      void compute_translation_diagnostics() noexcept {
        // 1. Compute new lengths

        // 2. Update tangents

        // 3. Compute new radius based on above lengths
        // Note! Here we need a CRTP method to get r from l
        self().dimension_from_volume_impl(get_volumes(), get_lengths());

        // 4. Compute dilatation of elements

        // 5. Compute dilatation of voronoi regions
      }

     protected:
      void compute_shear_strains() noexcept {
        // Q (et - d_3)
        // Q is always latest, only e and t need to be computed
        compute_translation_diagnostics();

        This::get<typename GeometryParent::ShearStretchStrainCollectionTag>() = 1.0;
      }

      void compute_curvature() noexcept {
        // kappa is the only rotational diagnostic, so we direcelty compute
        // that here
        // std::get<typename GeometryParent::CurvatureCollectionTag>(
        // self().data()) = 1.0;
        // get_curvature() = log(get_rotation_matrices());
      }
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
