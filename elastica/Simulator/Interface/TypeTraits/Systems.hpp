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

      // DONE Should be in a traits namespace somewhere

      /*
      // Version 1 : n instantiations, each with two types : makes it harder to
      use.

      template <typename SystemTag> struct SystemTagUnpacker { struct
      GetPrimitiveType { using type = typename SystemTag::PrimitiveType;
        };

        struct GetSystemCollectionType {
          using type = typename SystemTag::SystemCollectionType;
        };
      };

        template <typename SystemTag>
        struct IsValidSystemTag<
            SystemTag,
            cpp17::void_t<
                typename SystemTagUnpacker<
                    SystemTag>::GetSystemCollectionType::type,
                typename SystemTagUnpacker<SystemTag>::GetPrimitiveType::type>>
            : std::true_type {};
       */

      // Version 2 : 2n instantiations of single type, easier to use with TMPL
      template <typename SystemTag>
      struct GetPrimitive {
        using type = typename SystemTag::PrimitiveType;
      };

      template <typename SystemTag>
      struct GetSystemCollection {
        using type = typename SystemTag::SystemCollectionType;
      };

      namespace detail {

        // Helper struct to check if the environment tags are valid (aka should
        // have a valid SystemCollectionType and PrimitiveType members)
        template <typename SystemTag, typename = cpp17::void_t<>>
        struct IsValidSystemTag : std::false_type {};

        template <typename SystemTag>
        struct IsValidSystemTag<
            SystemTag,
            cpp17::void_t<typename GetSystemCollection<SystemTag>::type,
                          typename GetPrimitive<SystemTag>::type>>
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
