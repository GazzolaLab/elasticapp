#pragma once

#include <cstddef>
#include <utility>

#include "DataStructures/TaggedTuple.hpp"
#include "ErrorHandling/Assert.hpp"
#include "Systems/CosseratRods/Policies/GeometryPolicies/Types.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/StdHelpers.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    // EBO should apply
    template <typename RodTraits>
    struct RodGeometryBase {
     protected:
      using This = RodGeometryBase<RodTraits>;
      using Traits = RodTraits;

      //      struct PositionTag : MatrixTag<NodeTag>{
      //        // initialization requirements
      //        using InitializationSliceType = PositionSliceTag;
      //        template <Func>
      //        using InitializerType = PositionInitializer<Func>;
      //      };

     private:
      /* The final implementation need not know about this guy*/
      //      struct PositionSliceTag : Traits::NodeMatrixSlice {};
      //      struct DirectorSliceTag : Traits::ElementMatrixSlice {};

     protected:
      struct PositionTag : Traits::NodeMatrix {
        //        using SliceType = PositionSliceTag;
        using This = PositionTag;
        using Parent = typename Traits::NodeMatrix;
        using SliceType = typename Parent::template Slice<This>;
        template <typename Func>
        using InitializerType = tags::PositionProfile<Func>;
      };

      struct DirectorTag : Traits::ElementMatrix {
        using This = DirectorTag;
        using Parent = typename Traits::ElementMatrix;
        using SliceType = typename Parent::template Slice<This>;
        template <typename Func>
        using InitializerType = tags::DirectorProfile<Func>;
      };

      using MemberTags = tmpl::list<PositionTag, DirectorTag>;
      using SliceTags =
          tmpl::transform<MemberTags,
                          tmpl::bind<Traits::template AddSliceType, tmpl::_1>>;

      RodGeometryBase() = default;
      inline virtual ~RodGeometryBase() = default;

      template <typename... Members, typename... Initializers>
      void initialize(std::size_t n_elems, TaggedTuple<Members...>& members_,
                      std::tuple<Initializers...>&& initializers) {
        auto init = [n_elems, &members_,
                     initializers = std::forward<std::tuple<Initializers...>>(
                         initializers)](auto Is) {
          using Tag = tmpl::at_c<This::MemberTags, Is>;
          auto& member = Traits::template get_slice<Tag>(members_);
          auto& initializer =
              Traits::template get_initializer<Tag>(initializers);
          auto dofs = Tag::get_dofs(n_elems);
          for (std::size_t i = 0; i < dofs; ++i) {
            // TODO : This doesn't work on slices, consider having the slice
            // member in the initializer itself
            Tag::slice(member, i) = initializer(i);
          }
        };

        index_apply<tmpl::size<MemberTags>::value>(
            [&init](auto... Is) { EXPAND_PACK_LEFT_TO_RIGHT(init(Is)); });
      }
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
