//==============================================================================
/*!
//  \file
//  \brief Defines helpers for registering constraints in \elastica
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

  namespace constraints {

    namespace registration {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Tag type for registering constraints within \elastica
       */
      struct ConstraintsTag;
      /*! \endcond */
      //************************************************************************

    }  // namespace registration

  }  // namespace constraints

}  // namespace elastica

//******************************************************************************
/*!\brief Get registered constraints classes
 * \ingroup constraints
 *
 * \details
 * Helper macro for getting all constraints registered thus far
 */
#define ELASTICA_GET_REGISTERED_CONSTRAINTS \
  GET_REGISTERED_TYPES(::elastica::constraints::registration::ConstraintsTag);
//******************************************************************************

//******************************************************************************
/*!\brief Register a constraint class
 * \ingroup constraints
 *
 * \details
 * Helper macro for registering a constraint defined by
 * ELASTICA_DEFINE_CONSTRAINT()
 */
#define ELASTICA_REGISTER_CONSTRAINT(Type)                             \
  REGISTER_TYPE(::elastica::constraints::registration::ConstraintsTag, \
                Type) /* NOLINT */
//******************************************************************************

//#define ELASTICA_DEFINE_CONSTRAINT(class_name)
//
//  namespace elastica {
//    namespace constraints {
//      template <typename Registrars>
//      class class_name;
//    }
//  }
//  namespace Registrars {
//    using class_name =
//        Registration::Registrar<::elastica::constraints::class_name>;
//  }
//  ELASTICA_REGISTER_CONSTRAINT(Registrars::class_name)
//  template <typename Registrars = tmpl::list<Registrars::class_name>>
//  class ::elastica::constraints::class_name
//      : public ::elastica::constraints::Constraint<
//            Registrars, ::elastica::constraints::class_name<Registrars>>

//******************************************************************************
/*!\brief Define a \elastica constraint class
 * \ingroup constraints
 *
 * \details
 * Helper macro for defining a constraint class along with its registrars for
 * compile-time registration
 *
 * \usage
 * Instead of defining a class like usual like below:
 * \code
 * class MyConstraint{
 *   // ...details...
 * };
 * \endcode
 *
 * you should use
 * \code
 * ELASTICA_DEFINE_CONSTRAINT(MyConstraint){
 *   // ...details...
 * };
 * \endcode
 *
 * which hides the details of registration, CRTP etc. from the user.
 *
 * \note
 * You should use this only within ::elastica::constraints, else a compiler
 * error is generated.
 */
#define ELASTICA_DEFINE_CONSTRAINT(class_name)                              \
  template <typename Registrars> /* clang-tidy: macro arg in parentheses */ \
  class class_name;              /* NOLINT */                               \
                                                                            \
  namespace Registrars {                                                    \
    using class_name = /* NOLINT */                                         \
        ::Registration::Registrar<::elastica::constraints::class_name>;     \
  }                                                                         \
                                                                            \
  template <typename Registrars = tmpl::list<Registrars::class_name>>       \
  class class_name                                            /* NOLINT */  \
      : public Constraint<Registrars, class_name<Registrars>> /* NOLINT */
//******************************************************************************
