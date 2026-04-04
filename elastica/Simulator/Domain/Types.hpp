//==============================================================================
/*!
//  \file
//  \brief Types belonging to the Domain module
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

#include <type_traits>

///
#include "Simulator/Frames/EulerianFrame.hpp"
///
#include "Utilities/DefineTypes.h"
#include "Utilities/Requires.hpp"

namespace elastica {

  //////////////////////////////////////////////////////////////////////////////
  //
  // Forward declarations of domain types
  //
  //////////////////////////////////////////////////////////////////////////////

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  // Boundary Types
  struct IsUnbounded;
  struct IsPeriodic;

  // Span
  template <typename Real>
  struct Span;

  // Policy types
  template <typename Real>
  class PeriodicDomainPolicy;
  template <typename Real>
  class UnboundedDomainPolicy;

  namespace detail {
    template <typename Real, template <typename> class DomainPolicy,
              Requires<std::is_floating_point<Real>::value> = nullptr>
    class Domain;
  }

  namespace detail {
    template <detail::EulerianFrame::DirectionType Dir, typename BoundaryType,
              typename Real>
    struct BoundaryAlongWrapper;
  }  // namespace detail

  /*! \endcond */
  //****************************************************************************

  //============================================================================
  //
  //  ALIAS DECLARATIONS
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Alias declaration for a PeriodicDomain
   * \ingroup domain
   *
   * PeriodicDomain is \elastica 's implementation of a computational domain
   * with periodic boundary conditions. The intent for using this class is not
   * to enforce all boundary conditions are periodic : a subset of the direction
   * can be unbounded. The \elastica simulator takes care of these details.
   */
  using PeriodicDomain = typename detail::Domain<real_t, PeriodicDomainPolicy>;
  //****************************************************************************

  //****************************************************************************
  /*!\brief Alias declaration for an UnboundedDomain
   * \ingroup domain
   *
   * UnboundedDomain is \elastica 's implementation of a computational domain
   * with strictly unbounded boundary conditions in all three directions.
   */
  using UnboundedDomain =
      typename detail::Domain<real_t, UnboundedDomainPolicy>;
  //****************************************************************************

}  // namespace elastica