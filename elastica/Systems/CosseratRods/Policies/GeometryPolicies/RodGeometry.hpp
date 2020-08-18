#pragma once

#include <cstddef>
#include <utility>

#include "DataStructures/TaggedTuple.hpp"
#include "ErrorHandling/Assert.hpp"
#include "Systems/CosseratRods/Math/Tags.hpp"
#include "Systems/CosseratRods/Policies/GeometryPolicies/Types.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/StdHelpers.hpp"
#include "Utilities/TMPL.hpp"

// final
#include "Systems/CosseratRods/Policies/GeometryPolicies/RodGeometryBase.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    // TODO Check for EBO
    template <typename RodTraits>
    struct RodGeometry : RodGeometryBase<RodTraits>{
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
      struct ReferenceLengthsTag : Traits::ElementVector {
        using This = ReferenceLengthsTag;
        using Parent = typename Traits::ElementVector;
        using SliceType = typename Parent::template Slice<This>;
        template <typename Func>
        using InitializerType = tags::ReferenceLengthProfile<Func>;
      };

      struct ReferenceCurvaturesTag : Traits::VoronoiVector {
        using This = ReferenceCurvaturesTag;
        using Parent = typename Traits::ElementVector;
        using SliceType = typename Parent::template Slice<This>;
        template <typename Func>
        using InitializerType = tags::ReferenceCurvatureProfile<Func>;
      };

      /*
       * Computed Tags : Can be filled in
       */
      struct ReferenceVoronoiLengthsTag : Traits::VoronoiVector {
          using This = ReferenceVoronoiLengthsTag;
          using Parent = typename Traits::VoronoiVector;
          using SliceType = typename Parent::template Slice<This>;
      };
      struct ElementLengthsTag : Traits::ElementVector {
          using This = ElementLengthsTag;
          using Parent = typename Traits::ElementVector;
          using SliceType = typename Parent::template Slice<This>;
      };

      /*
      VoronoiLengths reference_voronoi_lengths_;

      // Current configuration
      ElementLengths lengths_, dilatations_;
      VoronoiLengths voronoi_lengths_, voronoi_dilatations_;
      Curvatures curvature_;
      Tangents tangents_;
       */

     private:
      // Can be generated, but whatever
      using GeneratedTags =
          tmpl::list<ReferenceLengthsTag, ReferenceCurvaturesTag>;
      using ComputedTags =
          tmpl::list<ReferenceVoronoiLengthsTag, ElementLengthsTag>;

      using ParentMemberTags = typename Parent::MemberTags;
      using ThisMemberTags = tmpl::append<GeneratedTags, ComputedTags>;

      using ParentSliceTags = typename Parent::SliceTags;
      using ThisSliceTags =
          tmpl::transform<ThisMemberTags,
                          tmpl::bind<Traits::template AddSliceType, tmpl::_1>>;

      static constexpr std::size_t get_number_of_generated_tags() noexcept {
        return tmpl::size<GeneratedTags>::value;
      }

     protected:
      using MemberTags = tmpl::append<ParentMemberTags, ThisMemberTags>;
      using SliceTags = tmpl::append<ParentSliceTags, ThisSliceTags>;

      RodGeometry() = default;
      inline virtual ~RodGeometry() = default;

      template <typename... Members, typename... Initializers>
      RodGeometry(std::size_t n_elems, TaggedTuple<Members...>& members_,
                  std::tuple<Initializers...>&& initializers)
          : Parent(n_elems, members_,
                   std::forward<std::tuple<Initializers...>>(initializers)) {
        // First initialize generators always
        auto init = [n_elems, &members_,
                     initializers = std::forward<std::tuple<Initializers...>>(
                         initializers)](auto Is) {
          using Tag = tmpl::at_c<ThisMemberTags, Is>;
          auto& member = Traits::template get_slice<Tag>(members_);
          auto& initializer =
              Traits::template get_initializer<Tag>(initializers);
          for (std::size_t i = 0; i < Tag::get_size(n_elems); ++i) {
            // TODO : This doesn't work on slices, consider having the slice
            // member in the initializer itself
            Tag::slice(member, i) = initializer(i);
          }
        };

        index_apply<get_number_of_generated_tags()>(
            [&init](auto... Is) { EXPAND_PACK_LEFT_TO_RIGHT(init(Is)); });

        // Next do the computed tags
        {
          auto& reference_voronoi_lengths =
              Traits::template get_slice<ReferenceVoronoiLengthsTag>(members_);
          reference_voronoi_lengths = 1.0;
        }

        {
          auto& reference_curvatures =
              Traits::template get_slice<ReferenceCurvaturesTag>(members_);
          reference_curvatures = 1.0;
        }
      }
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
