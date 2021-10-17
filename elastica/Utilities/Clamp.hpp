//==============================================================================
/*!
//  \file
//  \brief Implementation of c++-17 `std::clamp`
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
// #include "ErrorHandling/Assert.hpp"

namespace cpp17 {

  //****************************************************************************
  /* \brief Returns the value clamped between lo and hi.
   * \ingroup utils
   *
   * \param  val  A value of arbitrary type.
   * \param  lo   A lower limit of arbitrary type.
   * \param  hi   An upper limit of arbitrary type.
   * \return max(val, lo) if val < hi or min(val, hi) otherwise.
   */
  template <typename Tp>
  constexpr const Tp& clamp(const Tp& val, const Tp& lo, const Tp& hi) {
    // assert(!(hi < lo), "hi < lo");
    return (val < lo) ? lo : (hi < val) ? hi : val;
  }
  //****************************************************************************

  //****************************************************************************
  /* \brief Returns the value clamped between lo and hi.
   * \ingroup utils
   *
   * \param  val  A value of arbitrary type.
   * \param  lo   A lower limit of arbitrary type.
   * \param  hi   An upper limit of arbitrary type.
   * /param  comp  A comparison functor.
   * /return max(val, lo, comp) if comp(val, hi)
   *	      or min(val, hi, comp) otherwise.
   */
  template <typename Tp, typename Compare>
  constexpr const Tp& clamp(const Tp& val, const Tp& lo, const Tp& hi,
                            Compare comp) {
    // ELASTICAASSERT(!comp(hi, lo), "comp(hi) < comp(lo)");
    return comp(val, lo) ? lo : comp(hi, val) ? hi : val;
  }
  //****************************************************************************

}  // namespace cpp17
