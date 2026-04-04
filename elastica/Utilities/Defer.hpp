//==============================================================================
/*!
//  \file
//  \brief A global functor for defering a quantity to a later point in time
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
#include "Utilities/StaticConst.hpp"

namespace elastica {

  namespace detail {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Defer to user type
    // \ingroup utils
    //
    // \details
    // A type to mark deferment
    */
    struct impl_deferred {
      //**********************************************************************
      /*!\brief A sink for user deferred type
      //
      // \param Any iterable
      */
      template <class R>
      constexpr auto const& operator()(R&&) const noexcept {
        return static_const<detail::impl_deferred>::value;
      }
      //**********************************************************************
    };
    /*! \endcond */
    //************************************************************************

  }  // namespace detail

  // elastica::defer is a global function object!
  namespace {
    //**************************************************************************
    /*!\brief object denoting `deferred to user` concept
    // \ingroup utils
    //
    // \details
    // Can be used to defer quantities to a later point in time, in case the
    // program meaningfully support it, in \elastica
    //
    // \example
    // Defer to a later time
    // \snippet Test_Defer.cpp defer_example
    //
    // defer can be composed and return a defer
    // \snippet Test_Defer.cpp defer_composition_example
    //
    */
    constexpr auto const& defer = static_const<detail::impl_deferred>::value;
    //**************************************************************************
  }  // namespace

}  // namespace elastica
