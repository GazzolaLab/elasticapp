//==============================================================================
/*!
//  \file
//  \brief Defines helpers for registering forcing in \elastica
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Utilities/Registration.hpp"

namespace elastica {

  namespace forcing {

    namespace registration {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Tag type for registering forcing within \elastica
       */
      struct ForcingTag;
      /*! \endcond */
      //************************************************************************

    }  // namespace registration

  }  // namespace forcing

}  // namespace elastica

//******************************************************************************
/*!\brief Get registered forcing classes
 * \ingroup forcing
 *
 * \details
 * Helper macro for getting all forcings registered thus far
 */
#define ELASTICA_GET_REGISTERED_FORCINGS \
  GET_REGISTERED_TYPES(::elastica::forcing::registration::ForcingTag);
//******************************************************************************

//******************************************************************************
/*!\brief Register a forcing class
 * \ingroup forcing
 *
 * \details
 * Helper macro for registering a forcing defined by
 * ELASTICA_DEFINE_FORCING()
 */
#define ELASTICA_REGISTER_FORCING(Type)                        \
  REGISTER_TYPE(::elastica::forcing::registration::ForcingTag, \
                Type) /* NOLINT */

//******************************************************************************

//#define ELASTICA_DEFINE_FORCING(class_name)
//
//  namespace elastica {
//    namespace forcing {
//      template <typename Registrars>
//      class class_name;
//    }
//  }
//  namespace Registrars {
//    using class_name =
//        Registration::Registrar<::elastica::forcing::class_name>;
//  }
//  ELASTICA_REGISTER_FORCING(Registrars::class_name)
//  template <typename Registrars = tmpl::list<Registrars::class_name>>
//  class ::elastica::forcing::class_name
//      : public ::elastica::forcing::Constraint<
//            Registrars, ::elastica::forcing::class_name<Registrars>>

//******************************************************************************
/*!\brief Define a \elastica forcing class
 * \ingroup forcing
 *
 * \details
 * Helper macro for defining a forcing class along with its registrars for
 * compile-time registration
 *
 * \usage
 * Instead of defining a class like usual like below:
 * \code
 * class MyForcing{
 *   //  ...details...
 * };
 * \endcode
 * you should use
 * \code
 * ELASTICA_DEFINE_FORCING(MyForcing){
 *   //  ...details...
 * };
 * \endcode
 *
 * which hides the details of registration, CRTP etc. from the user.
 *
 * \note
 * You should use this only within ::elastica::forcing, else a compiler error
 * is generated
 */
#define ELASTICA_DEFINE_FORCING(class_name)                             \
  template <typename Registrars>                                        \
  class class_name; /* NOLINT */                                        \
                                                                        \
  namespace Registrars {                                                \
    using class_name = /* NOLINT */                                     \
        ::Registration::Registrar<::elastica::forcing::class_name>;     \
  }                                                                     \
                                                                        \
  template <typename Registrars = tmpl::list<Registrars::class_name>>   \
  class class_name                                         /* NOLINT */ \
      : public Forcing<Registrars, class_name<Registrars>> /* NOLINT */
//******************************************************************************
