#pragma once

#include <cstddef>
#include <iostream>
#include <utility>

#include "ErrorHandling/Assert.hpp"
#include "Systems/CosseratRods/CosseratRodTraitMacros.hpp"
#include "Systems/CosseratRods/Policies/StrongTypes.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace detail_cosserat_rod {

    // EBO should apply
    template <typename RodTraits>
    class RodGeometryBase {
     protected:
      using This = RodGeometryBase<RodTraits>;
      using Traits = RodTraits;

     protected:
      ELASTICA_BLOCK_REGISTER_INITIALIZED_TYPE(PositionCollectionTag, NodeTag,
                                               MatrixTag,
                                               tags::PositionProfileTag);
      // DONE : Complete with Rank3 (tensor)
      ELASTICA_BLOCK_REGISTER_INITIALIZED_TYPE(DirectorCollectionTag,
                                               ElementTag, TensorTag,
                                               tags::DirectorProfileTag);

      using MemberTags =
          tmpl::list<PositionCollectionTag, DirectorCollectionTag>;
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

      template <typename CosseratSlice, typename CosseratInitializer>
      void initialize(CosseratSlice& property_slices,
                      CosseratInitializer&& initializers) {
        //        https://stackoverflow.com/a/53144382
        struct {
          CosseratInitializer value;
        } cap{std::forward<CosseratInitializer>(initializers)};
        // Note : need a mutable lambda as we can now "move" from cap
        tmpl::for_each<MemberTags>([&property_slices, cap = std::move(cap)](
                                       auto member_tag) mutable -> void {
          using MemberTag = tmpl::type_from<decltype(member_tag)>;

          auto& member(property_slices.template get<MemberTag>());
          // TODO : proper move semantics
          auto&& initializer(cap.value.template get<MemberTag>());

          const auto dofs = MemberTag::get_dofs(cap.value.n_elems);
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
