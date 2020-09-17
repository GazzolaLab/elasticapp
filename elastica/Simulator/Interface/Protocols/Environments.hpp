#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <type_traits>
#include <utility>

#include "Simulator/Interface/TypeTraits/Aliases/Environments.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace elastica {

  namespace configuration {

    namespace protocols {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Environments configuration protocol.
      // \ingroup protocols
      //
      // Class to enforce adherence to Environments configuration protocol. Any
      // valid environment configuration within the Elastica++ library should
      // (publicly) inherit from this class to indicate it qualifies as a
      // Environment. Only in case a class is derived publicly from this base
      // class, the \c tt::conforms_to and \c tt::assert_conforms_to type traits
      // recognizes the class as valid Environments.
      //
      // Requires that conforming type has these nested types:
      // - \a AdmissibleEnvironmentTags : A (strictly) tmpl::list of tags of
      //                                  admissible environments
      // - \a AdmissibleImposedPrimitives : A (strictly) tmpl::list of
      //                                    temporally non-evolving primitives
      */
      struct EnvironmentsConfiguration {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\brief Auxiliary helper struct for enforcing protocols.
        // \ingroup protocols
        */
        template <typename ConformingType>
        struct test {
         public:
          // Check for nested types of the ConfirmingType
          static_assert(
              ::tt::is_detected_v<::elastica::tt::admissible_environment_tags_t,
                                  ConformingType>,
              "Not a conforming environments configuration, doesn't have a "
              "nested AdmissibleEnvironmentTags member");
          static_assert(
              ::tt::is_a_v<tmpl::list,
                           ::elastica::tt::admissible_environment_tags_t<
                               ConformingType>>,
              "Admissible environment tags should belong in a typelist!");

          static_assert(
              ::tt::is_detected_v<
                  ::elastica::tt::admissible_imposed_primitives_t,
                  ConformingType>,
              "Not a conforming environments configuration, doesn't have a "
              "nested AdmissibleImposedPrimitives member");
          static_assert(
              ::tt::is_a_v<tmpl::list,
                           ::elastica::tt::admissible_imposed_primitives_t<
                               ConformingType>>,
              "Admissible imposed primitives should belong in a typelist!");
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace defaults

}  // namespace elastica
