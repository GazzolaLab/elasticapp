#pragma once

#include <cstddef>
#include <utility>

#include "ErrorHandling/Assert.hpp"
#include "Systems/CosseratRods/CosseratRodTraitMacros.hpp"
#include "Systems/CosseratRods/Policies/StrongTypes.hpp"
#include "Systems/CosseratRods/TypeTraits/detail.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/TMPL.hpp"

// final
#include "Systems/CosseratRods/Policies/GeometryPolicies/RodGeometryBase.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    // TODO Check for EBO
    template <typename RodTraits>
    class RodGeometry : public RodGeometryBase<RodTraits> {
     protected:
      using Parent = RodGeometryBase<RodTraits>;
      using This = RodGeometry<RodTraits>;
      using Traits = RodTraits;

     private:
      /* The final implementation need not know about this guy*/
      //      struct ReferenceLengthsSliceTag : Traits::template
      //      VectorSliceTag<NodeTag> {};

     protected:
      /*
       * Generative Tags : To be filled in by generators
       */
      struct ReferenceElementLengthCollectionTag : Traits::ElementVector {
        using This = ReferenceElementLengthCollectionTag;
        using Parent = typename Traits::ElementVector;
        using SliceType = typename Parent::template Slice<This>;
        using InitializerTag = tags::ReferenceLengthProfileTag;

        // Override ghosts as reference lengths should be +ve
        static inline constexpr auto ghost_value() -> real_t {
          return real_t(1.0);
        }
      };

      ELASTICA_BLOCK_REGISTER_INITIALIZED_TYPE(
          ReferenceCurvatureCollectionTag, VoronoiTag, MatrixTag,
          tags::ReferenceCurvatureProfileTag);

      /*
       * Computed Tags : Can be filled in
       */
      struct ReferenceVoronoiLengthCollectionTag : Traits::VoronoiVector {
        using This = ReferenceVoronoiLengthCollectionTag;
        using Parent = typename Traits::VoronoiVector;
        using SliceType = typename Parent::template Slice<This>;

        // Override ghosts as reference lengths should be +ve
        static inline constexpr auto ghost_value() -> real_t {
          return real_t(100.0);
        }
      };

      ELASTICA_BLOCK_REGISTER_COMPUTED_TYPE(ElementLengthCollectionTag,
                                            ElementTag, VectorTag);
      ELASTICA_BLOCK_REGISTER_COMPUTED_TYPE(ElementDilatationCollectionTag,
                                            ElementTag, VectorTag);
      ELASTICA_BLOCK_REGISTER_COMPUTED_TYPE(VoronoiLengthCollectionTag,
                                            VoronoiTag, VectorTag);
      ELASTICA_BLOCK_REGISTER_COMPUTED_TYPE(VoronoiDilatationCollectionTag,
                                            VoronoiTag, VectorTag);
      ELASTICA_BLOCK_REGISTER_COMPUTED_TYPE(CurvatureCollectionTag, VoronoiTag,
                                            MatrixTag);
      ELASTICA_BLOCK_REGISTER_COMPUTED_TYPE(TangentCollectionTag, ElementTag,
                                            MatrixTag);
      ELASTICA_BLOCK_REGISTER_COMPUTED_TYPE(ShearStretchStrainCollectionTag,
                                            ElementTag, VectorTag);

     private:
      // Can be generated, but whatever
      using ThisMemberTags = tmpl::list<
          ReferenceElementLengthCollectionTag, ReferenceCurvatureCollectionTag,
          ReferenceVoronoiLengthCollectionTag, ElementLengthCollectionTag,
          ElementDilatationCollectionTag, VoronoiLengthCollectionTag,
          VoronoiDilatationCollectionTag, CurvatureCollectionTag,
          TangentCollectionTag, ShearStretchStrainCollectionTag>;

      using GeneratedTags =
          tmpl::filter<ThisMemberTags,
                       tt::CheckForInitializerTagType<tmpl::_1>>;
      using ComputedTags = tmpl::list_difference<ThisMemberTags, GeneratedTags>;

      using ParentMemberTags = typename Parent::MemberTags;

      /*
      using ParentSliceTags = typename Parent::SliceTags;
      using ThisSliceTags =
          tmpl::transform<ThisMemberTags,
                          tmpl::bind<Traits::template AddSliceType, tmpl::_1>>;
      */

      /*
      static constexpr std::size_t get_number_of_generated_tags() noexcept {
        return tmpl::size<GeneratedTags>::value;
      }
      */

     protected:
      using MemberTags = tmpl::append<ParentMemberTags, ThisMemberTags>;
      //      using SliceTags = tmpl::append<ParentSliceTags, ThisSliceTags>;

      /*
      RodGeometry() = default;
      inline ~RodGeometry() = default;
      using Parent::print;
      */

      template <class InitializerTuple, typename... Members>
      void initialize(std::size_t n_elems, std::tuple<Members...>& members_,
                      InitializerTuple&& initializers) {
        // 0. using slice class
        auto get_slice = [&members_](auto tag) -> decltype(auto) {
          return Traits::template get_slice<decltype(tag)>(members_);
        };

        // 1. Initialize parent
        Parent::initialize(n_elems, members_,
                           std::forward<InitializerTuple>(initializers));

        // 2. Initialize generators first
        struct {
          InitializerTuple value;
        } cap{std::forward<InitializerTuple>(initializers)};
        tmpl::for_each<This::GeneratedTags>(
            [n_elems, &members_,
             cap = std::move(cap)](auto generated_member_tag) mutable -> void {
              using GeneratedMemberTag =
                  tmpl::type_from<decltype(generated_member_tag)>;

              auto& member =
                  Traits::template get_slice<GeneratedMemberTag>(members_);
              auto&& initializer =
                  Traits::template get_initializer<GeneratedMemberTag>(
                      std::forward<InitializerTuple>(cap.value));

              auto dofs = GeneratedMemberTag::get_dofs(n_elems);
              for (std::size_t i = 0; i < dofs; ++i) {
                // TODO : This doesn't work on slices, consider having the
                // slice member in the initializer itself
                GeneratedMemberTag::slice(member, i) = initializer(i);
              }
            });

        // 3. Finally do the computed tags

        //            TangentCollectionTag>;
        {
          auto& reference_voronoi_length_collection =
              get_slice(ReferenceVoronoiLengthCollectionTag{});
          reference_voronoi_length_collection =
              Traits::calculate_voronoi_length(
                  get_slice(ReferenceElementLengthCollectionTag{}));
        }

        {
          auto& element_length_collection =
              get_slice(ElementLengthCollectionTag{});
          element_length_collection = 2.0;
        }

        //        {
        //          //          dilatations_ = lengths_ / reference_lengths_;
        //          get_slice(ElementDilatationCollectionTag{}) =
        //              get_slice(ElementLengthCollectionTag{}) /
        //              get_slice(ReferenceElementLengthCollectionTag{});
        //        }
        //
        {
          auto& voronoi_length_collection =
              get_slice(VoronoiLengthCollectionTag{});
          voronoi_length_collection = 1.0;
        }

        //        {
        //          get_slice(VoronoiDilatationCollectionTag{}) =
        //              get_slice(VoronoiLengthCollectionTag{}) /
        //              get_slice(ReferenceVoronoiLengthCollectionTag{});
        //        }
        //
        {
          auto& curvature_collection = get_slice(CurvatureCollectionTag{});
          curvature_collection = 1.0;
        }

        {
          auto& tangent_collection = get_slice(TangentCollectionTag{});
          tangent_collection = 1.0;
        }

        {
          auto& shear_stretch_strains_collection =
              get_slice(ShearStretchStrainCollectionTag{});
          shear_stretch_strains_collection = 1.0;
        }
      }
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
