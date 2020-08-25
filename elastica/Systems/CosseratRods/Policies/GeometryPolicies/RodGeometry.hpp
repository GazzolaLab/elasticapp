#pragma once

#include <cstddef>
#include <utility>

#include "ErrorHandling/Assert.hpp"
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
    struct RodGeometry : RodGeometryBase<RodTraits> {
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

      struct ReferenceCurvatureCollectionTag : Traits::VoronoiVector {
        using This = ReferenceCurvatureCollectionTag;
        using Parent = typename Traits::VoronoiVector;
        using SliceType = typename Parent::template Slice<This>;
        using InitializerTag = tags::ReferenceCurvatureProfileTag;
        //        template <typename Func>
        //        using InitializerType = tags::ReferenceCurvatureProfile<Func>;
      };

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

      struct ElementLengthCollectionTag : Traits::ElementVector {
        using This = ElementLengthCollectionTag;
        using Parent = typename Traits::ElementVector;
        using SliceType = typename Parent::template Slice<This>;
      };

      struct ElementDilatationCollectionTag : Traits::ElementVector {
        using This = ElementDilatationCollectionTag;
        using Parent = typename Traits::ElementVector;
        using SliceType = typename Parent::template Slice<This>;
      };

      struct VoronoiLengthCollectionTag : Traits::VoronoiVector {
        using This = VoronoiLengthCollectionTag;
        using Parent = typename Traits::VoronoiVector;
        using SliceType = typename Parent::template Slice<This>;
      };

      struct VoronoiDilatationCollectionTag : Traits::VoronoiVector {
        using This = VoronoiDilatationCollectionTag;
        using Parent = typename Traits::VoronoiVector;
        using SliceType = typename Parent::template Slice<This>;
      };

      struct CurvatureCollectionTag : Traits::VoronoiMatrix {
        using This = CurvatureCollectionTag;
        using Parent = typename Traits::VoronoiMatrix;
        using SliceType = typename Parent::template Slice<This>;
      };

      struct TangentCollectionTag : Traits::ElementMatrix {
        using This = TangentCollectionTag;
        using Parent = typename Traits::ElementMatrix;
        using SliceType = typename Parent::template Slice<This>;
      };

     private:
      // Can be generated, but whatever
      using ThisMemberTags =
          tmpl::list<ReferenceElementLengthCollectionTag,
                     ReferenceCurvatureCollectionTag,
                     ReferenceVoronoiLengthCollectionTag,
                     ElementLengthCollectionTag, ElementDilatationCollectionTag,
                     VoronoiLengthCollectionTag, VoronoiDilatationCollectionTag,
                     CurvatureCollectionTag, TangentCollectionTag>;

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
        tmpl::for_each<GeneratedTags>(
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
          element_length_collection = 1.0;
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
      }
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
