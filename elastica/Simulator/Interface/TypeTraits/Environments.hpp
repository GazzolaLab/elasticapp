#pragma once

#include <type_traits>

#include "elastica/Utilities/ListRequires.hpp"
#include "elastica/Utilities/Requires.hpp"
#include "elastica/Utilities/TMPL.hpp"
#include "elastica/Utilities/TypeTraits.hpp"

namespace elastica {

  namespace interface {

    namespace tt {

      // DONE Should be in a traits namespace somewhere
      template <typename C, typename = cpp17::void_t<>>
      struct IsEnvironmentConfiguration : std::false_type {};

      // Check for
      // 1. Existence of members
      template <typename C>
      struct IsEnvironmentConfiguration<
          C, cpp17::void_t<typename C::AdmissibleEnvironmentTags,
                           typename C::AdmissibleImposedPrimitives>>
          : std::true_type {};

      namespace detail {

        // Helper struct to check if the environment tags are valid (aka should
        // have a templated type member)
        template <typename EnvironmentTag, typename = ::tt::templated_void_t<>>
        struct IsValidEnvironmentTag : std::false_type {};

        template <typename EnvironmentTag>
        struct IsValidEnvironmentTag<
            EnvironmentTag,
            ::tt::templated_void_t<EnvironmentTag::template type>>
            : std::true_type {};

      }  // namespace detail

      // Check for
      // 1. They should always be tmpl::lists
      // 2. The environment tags should be valid ()
      // We have two requirements here rather than one
      template <typename EC,
                Requires<::tt::is_a_v<tmpl::list,
                                      typename EC::AdmissibleEnvironmentTags>> =
                    nullptr>
      struct IsValidEnvironmentConfiguration : std::true_type {
        using AdmissibleEnvironmentTags =
            typename EC::AdmissibleEnvironmentTags;
        using Constraint = ListRequires<detail::IsValidEnvironmentTag>;
        using RequiresType =
            tmpl::transform<AdmissibleEnvironmentTags,
                            Constraint::template apply<tmpl::_1>>;
      };

    }  // namespace tt

  }  // namespace interface

}  // namespace elastica

//        // Helper struct to ease going through the typelist
//        // Also shows where the error is
//        template <typename ET>
//        struct ValidEnvironmentConfigurationHelper {
//          using type = Requires<IsValidEnvironmentTag<ET>::value>;
//        };

// Helps validate every tag one by one to give a cleaner error message
//        template <typename... EnvTags>
//        struct ValidEnvironmentConfigurationWrapper {
//          using type =
//          cpp17::conjunction<Requires<IsValidEnvironmentConfiguration<EnvTags>::value>...>;
//        };
//

//      namespace detail {
//        // true_type or false_type
//        template <typename... EnvTags>
//        using ValidEnvironmentConfigurationWrapper = cpp17::conjunction<
//            Requires<IsValidEnvironmentTag<EnvTags>::value>...>;
//
//        template <typename L>
//        using ValidEnvironmentConfigurationHelper =
//            tmpl::wrap<L, ValidEnvironmentConfigurationWrapper>;
//      }  // namespace detail
