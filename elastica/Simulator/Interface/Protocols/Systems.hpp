#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <type_traits>
#include <utility>

#include "Simulator/Interface/TypeTraits/Aliases/Systems.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace configuration {

    namespace protocols {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*!\brief Systems configuration protocol.
      // \ingroup configuration protocols
      //
      // Class to enforce adherence to Systems configuration protocol. Any valid
      // system configuration within the Elastica++ library should
      // (publicly) inherit from this class to indicate it qualifies as a
      // System Configuration. Only in case a class is derived publicly from
      // this base class, the \c tt::conforms_to and \c tt::assert_conforms_to
      // type traits recognizes the class as valid System Configuration.
      //
      // Requires that conforming type has these nested types:
      // - \a AdmissibleSystemTags : List of tags of admissible systems
      */
      struct SystemsConfiguration {
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
              ::tt::is_detected_v<::elastica::tt::admissible_system_tags_t,
                                  ConformingType>,
              "Not a conforming domain configuration, doesn't have a "
              "nested AdmissibleSystemTags member");
          static_assert(
              ::tt::is_a_v<tmpl::list,
                  ::elastica::tt::admissible_system_tags_t<
                      ConformingType>>,
              "Admissible system tags should belong in a typelist!");
        };
        /*! \endcond */
        //**********************************************************************
      };
      //************************************************************************

    }  // namespace protocols

  }  // namespace defaults

}  // namespace elastica
