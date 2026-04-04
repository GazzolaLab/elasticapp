//==============================================================================
/*!
//  \file
//  \brief Defines helpers for registering connections in \elastica
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "Utilities/Registration.hpp"

namespace elastica {

  namespace connections {

    namespace registration {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Tag type for registering connections within \elastica
       */
      struct ConnectionsTag;
      /*! \endcond */
      //************************************************************************

    }  // namespace registration

  }  // namespace connections

}  // namespace elastica

//******************************************************************************
/*!\brief Get registered connections classes
 * \ingroup connections
 *
 * \details
 * Helper macro for getting all connections registered thus far
 */
#define ELASTICA_GET_REGISTERED_CONNECTIONS \
  GET_REGISTERED_TYPES(::elastica::connections::registration::ConnectionsTag);
//******************************************************************************

//******************************************************************************
/*!\brief Register a connection class
 * \ingroup connections
 *
 * \details
 * Helper macro for registering a connection defined by
 * ELASTICA_DEFINE_CONNECTION()
 */
#define ELASTICA_REGISTER_CONNECTION(Type)                             \
  REGISTER_TYPE(::elastica::connections::registration::ConnectionsTag, \
                Type) /* NOLINT */
//******************************************************************************

//#define ELASTICA_DEFINE_CONNECTION(class_name)
//
//  namespace elastica {
//    namespace connections {
//      template <typename Registrars>
//      class class_name;
//    }
//  }
//  namespace Registrars {
//    using class_name =
//        Registration::Registrar<::elastica::connections::class_name>;
//  }
//  ELASTICA_REGISTER_CONNECTION(Registrars::class_name)
//  template <typename Registrars = tmpl::list<Registrars::class_name>>
//  class ::elastica::connections::class_name
//      : public ::elastica::connections::Connection<
//            Registrars, ::elastica::connections::class_name<Registrars>>

//******************************************************************************
/*!\brief Define a \elastica connection class
 * \ingroup connections
 *
 * \details
 * Helper macro for defining a connection class along with its registrars for
 * compile-time registration
 *
 * \usage
 * Instead of defining a class like usual like below:
 * \code
 * class MyConnection{
 *   // ...details...
 * };
 * \endcode
 *
 * you should use
 * \code
 * ELASTICA_DEFINE_CONNECTION(MyConnection){
 *   // ...details...
 * };
 * \endcode
 *
 * which hides the details of registration, CRTP etc. from the user.
 *
 * \note
 * You should use this only within ::elastica::connections, else a compiler
 * error is generated.
 */
#define ELASTICA_DEFINE_CONNECTION(class_name)                              \
  template <typename Registrars> /* clang-tidy: macro arg in parentheses */ \
  class class_name;              /* NOLINT */                               \
                                                                            \
  namespace Registrars {                                                    \
    using class_name = /* NOLINT */                                         \
        ::Registration::Registrar<::elastica::connections::class_name>;     \
  }                                                                         \
                                                                            \
  template <typename Registrars = tmpl::list<Registrars::class_name>>       \
  class class_name                                            /* NOLINT */  \
      : public Connection<Registrars, class_name<Registrars>> /* NOLINT */
//******************************************************************************
