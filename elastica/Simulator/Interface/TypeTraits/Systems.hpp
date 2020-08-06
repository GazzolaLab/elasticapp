#pragma once

#include <type_traits>

#include "elastica/Utilities/ListRequires.hpp"
#include "elastica/Utilities/TMPL.hpp"
#include "elastica/Utilities/TypeTraits.hpp"

namespace elastica {

  namespace interface {

    namespace tt {

      // DONE Should be in a traits namespace somewhere
      template <typename C, typename = void>
      struct IsSystemConfiguration : std::false_type {};

      // Check for
      // 1. Existence of members
      template <typename C>
      struct IsSystemConfiguration<
          C, cpp17::void_t<typename C::AdmissibleSystemCollectionTags>>
          : std::true_type {};

      namespace detail {

        // Helper struct to check if the environment tags are valid (aka should
        // have a valid SystemCollectionType and PrimitiveType members)
        template <typename SystemTag, typename = cpp17::void_t<>>
        struct IsValidSystemTag : std::false_type {};

        template <typename SystemTag>
        struct IsValidSystemTag<
            SystemTag, cpp17::void_t<typename SystemTag::SystemCollectionType,
                                     typename SystemTag::PrimitiveType>>
            : std::true_type {};

      }  // namespace detail

      // Check for
      // 1. The system tag should be valid ()
      template <typename SC>
      struct IsValidSystemConfiguration : std::true_type {
        using AdmissibleSystemCollectionTags =
            typename SC::AdmissibleSystemCollectionTags;
        using Constraint = ListRequires<detail::IsValidSystemTag>;
        using RequiresType =
            tmpl::transform<AdmissibleSystemCollectionTags,
                            Constraint::template apply<tmpl::_1>>;
      };

    }  // namespace tt

  }  // namespace interface

}  // namespace elastica
