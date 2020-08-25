#pragma once

#include <cstddef>
#include <iostream>
#include <utility>

#include "ErrorHandling/Assert.hpp"
#include "Systems/CosseratRods/Policies/StrongTypes.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    // EBO should apply
    template <typename RodTraits>
    struct RodGeometryBase {
     protected:
      using This = RodGeometryBase<RodTraits>;
      using Traits = RodTraits;

     private:
      /* The final implementation need not know about this guy*/
      //      struct PositionSliceTag : Traits::NodeMatrixSlice {};
      //      struct DirectorSliceTag : Traits::ElementMatrixSlice {};

     protected:
      struct PositionCollectionTag : Traits::NodeMatrix {
        //        using SliceType = PositionCollectionSliceTag;
        using This = PositionCollectionTag;
        using Parent = typename Traits::NodeMatrix;
        using SliceType = typename Parent::template Slice<This>;
        using InitializerTag = tags::PositionProfileTag;
        /*
        //        template <typename Func>
        //        using InitializerType = tags::PositionProfile<Func>;
         */
      };

      struct DirectorCollectionTag : Traits::ElementMatrix {
        using This = DirectorCollectionTag;
        using Parent = typename Traits::ElementMatrix;
        using SliceType = typename Parent::template Slice<This>;
        using InitializerTag = tags::DirectorProfileTag;

        /*
        //        template <typename Func>
        //        using InitializerType = tags::DirectorProfile<Func>;
        */
      };

      using MemberTags = tmpl::list<PositionCollectionTag, DirectorCollectionTag>;
      /*
      using SliceTags =
          tmpl::transform<MemberTags,
                          tmpl::bind<Traits::template AddSliceType, tmpl::_1>>;
      using NodeMemberTags =
          tmpl::filter<MemberTags,
                       typename Traits::template IsNodeStaggered<tmpl::_1>>;
      using ElementMemberTags =
          tmpl::filter<MemberTags,
                       typename Traits::template IsElementStaggered<tmpl::_1>>;
                       */

      /*
      RodGeometryBase() = default;
      inline ~RodGeometryBase() = default;
       */

      template <typename Init>
      void print(Init const&) {
        std::cout << "const &" << std::endl;
      }

      template <typename Init>
      void print(Init&) {
        std::cout << "&" << std::endl;
      }

      template <typename Init>
      void print(Init&&) {
        std::cout << "&&" << std::endl;
      }

      template <class InitializerTuple, typename... Members>
      void initialize(std::size_t n_elems, std::tuple<Members...>& members_,
                      InitializerTuple&& initializers) {
        //        https://stackoverflow.com/a/53144382
        struct {
          InitializerTuple value;
        } cap{std::forward<InitializerTuple>(initializers)};
        // Note : need a mutable lambda as we can now "move" from cap
        tmpl::for_each<MemberTags>([this, n_elems, &members_, cap = std::move(cap)](
                                       auto member_tag) mutable -> void {
          using MemberTag = tmpl::type_from<decltype(member_tag)>;
          auto& member = Traits::template get_slice<MemberTag>(members_);
          // TODO initializer here is never a &&, but rather a &
          //  is it because of wrapping it in a NamedType?
          auto initializer = Traits::template get_initializer<MemberTag>(
              std::forward<InitializerTuple>(cap.value));
          const auto dofs = MemberTag::get_dofs(n_elems);
          for (std::size_t i = 0; i < dofs; ++i) {
            // TODO : This doesn't work on slices, consider having the slice
            // member in the initializer itself
            MemberTag::slice(member, i) = initializer(i);
          }
        });
      }
    };

  }  // namespace detail_cosserat_rod

}  // namespace elastica
