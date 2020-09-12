#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <type_traits>
#include <utility>

#include "ModuleSettings/Environments/Aliases.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace elastica {

  namespace defaults {

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
      // Class to enforce adherence to Environments protocol. Any valid
      // environment configuration within the Elastica++ library should
      // (publicly) inherit from this class to indicate it qualifies as a
      // Environment. Only in case a class is derived publicly from this base
      // class, the `tt::conforms_to` and `tt::assert_conforms_to` type traits
      // recognizes the class as valid Environments.
      //
      // Requires that conforming type has these nested types:
      // - `AdmissibleEnvironmentTags`: List of tags of admissible environments
      // - `AdmissibleImposedPrimitives`: List of non-evolving primitives
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
              "Not a conforming domain configuration, doesn't have a "
              "nested AdmissibleEnvironmentTags member");

          static_assert(::tt::is_detected_v<
                            ::elastica::tt::admissible_imposed_primitives_t,
                            ConformingType>,
                        "Not a conforming domain configuration, doesn't have a "
                        "nested AdmissibleImposedPrimitives member");
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace defaults

}  // namespace elastica
