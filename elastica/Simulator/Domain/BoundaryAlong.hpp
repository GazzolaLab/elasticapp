//==============================================================================
/*!
//  \file
//  \brief A wrapper around Span and BoundaryTypes for easy interface
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

#include <type_traits>

#include "Simulator/Domain/BoundaryTypes.hpp"
#include "Simulator/Domain/Span.hpp"
#include "Simulator/Domain/Types.hpp"
#include "Simulator/Frames/EulerianFrame.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Infinity.hpp"  // inf

namespace elastica {

  namespace detail {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Helper wrapping a span along with a direction and boundary
     * condition
     * \ingroup domain
     *
     * BoundaryAlongWrapper represents a physical span extending up to the
     * domain in the dimension denoted by `Dir`.
     *
     * \tparam Dir Direction of boundary (one of x, y, z, see
     * elastica::detail::EulerianFrame)
     * \tparam BoundaryType The type of boundary, one of elastica::IsUnbounded
     * or elastica::IsPeriodic
     * \tparam Real Any floating point type
     */
    template <detail::EulerianFrame::DirectionType Dir, typename BoundaryType,
              typename Real>
    struct BoundaryAlongWrapper : public Span<Real> {
      //**Type definitions******************************************************
      //!< Parent type
      using Parent = Span<Real>;
      //************************************************************************

      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      using Parent::Parent;
      explicit BoundaryAlongWrapper(Span<Real> other) noexcept
          : Parent(std::move(other)){};
      //@}
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace detail

  //****************************************************************************
  /*!\brief Marks a periodic boundary type and span along a given direction
   * \ingroup domain
   *
   * \example
   * \snippet Test_BoundaryAlong.cpp periodic_eg
   *
   * \tparam Dir Direction of boundary (one of x, y, z, see
   * elastica::detail::EulerianFrame)
   * \tparam BoundaryType The type of boundary, should be explictly specified to
   * elastica::IsPeriodic
   * \param arg_min (expected) minimum value of the periodic span
   * \param arg_max (expected) maximum value of the periodic span
   * \return A struct marking a periodic boundary along Dir
   */
  template <detail::EulerianFrame::DirectionType Dir, typename BoundaryType,
            typename Real>
  inline constexpr decltype(auto) boundary_along(
      Real arg_min, Real arg_max, BoundaryType = IsPeriodic{}) noexcept {
    using RT = const detail::BoundaryAlongWrapper<Dir, IsPeriodic, Real>;
    return RT{make_span(arg_min, arg_max)};
  }
  //****************************************************************************

  //****************************************************************************
  /*!\brief Marks a unbounded boundary type with infinite span along a given
   * direction
   * \ingroup domain
   *
   * \example
   * \snippet Test_BoundaryAlong.cpp unbounded_eg
   *
   * \tparam Dir Direction of boundary (one of x, y, z, see
   * elastica::detail::EulerianFrame)
   * \tparam BoundaryType The type of boundary, should be explictly specified to
   * elastica::IsUnbounded
   * \return A struct marking a unbounded boundary along Dir
   */
  template <detail::EulerianFrame::DirectionType Dir, typename BoundaryType>
  inline constexpr decltype(auto) boundary_along(
      BoundaryType = IsUnbounded{}) noexcept {
    using RT = detail::BoundaryAlongWrapper<Dir, IsUnbounded, real_t>;
    // here we need a large number, but its not used anyway within the main
    // domain (as it is inf there too).
    // we can use inf, but -inf and inf are different types
    constexpr real_t some_large_number = inf;  // -123456789.123456789;
    return RT{make_span(-some_large_number, some_large_number)};
  }
  //****************************************************************************

}  // namespace elastica