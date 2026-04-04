//==============================================================================
/*!
//  \file
//  \brief Traits for Affordances for Named types
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

// A modified version of
// https://github.com/joboccara/NamedType/blob/master/named_type_impl.hpp
// customized for elastica needs
// See https://raw.githubusercontent.com/joboccara/NamedType/master/LICENSE

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

namespace named_type {

  //****************************************************************************
  /*!\brief Traits class for Affordances
  // \ingroup named_type
  //
  // \details
  // The Traits class express requirements on the value type `T` of a NamedType
  // It must contain a nested alias template `type`, templated on this `T`, to
  // be used in a SFINAE context
  //
  // \example
  // (NamedType affordance examples live in unit tests.)
  //
  // \tparam A Any affordance defined using the CRTP pattern
  */
  template <template <typename> class A>
  struct Traits;
  //****************************************************************************

  template <template <typename> class A, typename F>
  using TraitsWizard = typename Traits<A>::template type<F>;

}  // namespace named_type