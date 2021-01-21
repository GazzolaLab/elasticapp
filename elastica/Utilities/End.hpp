//==============================================================================
/*!
//  \file
//  \brief A global functor for end of an iterable
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
//
//  The key idea was taken from the Range v3 library with the following
//  copyrights
**
**  Copyright Eric Niebler 2013-present
**
**  Use, modification and distribution is subject to the
**  Boost Software License, Version 1.0. (See accompanying
**  file LICENSE_1_0.txt or copy at
**  http://www.boost.org/LICENSE_1_0.txt)
**
**  Project home: https://github.com/ericniebler/range-v3
**
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************

#pragma once

#include <cassert>
#include <utility>

#include "Utilities/ForceInline.hpp"
#include "Utilities/StaticConst.hpp"

// end proxy
namespace elastica {

  namespace detail {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief End type
    // \ingroup utils
    //
    // \details
    // Type of the global end functor for any iterable
    */
    struct impl_end_fn {
      //**********************************************************************
      /*!\brief Get the end of any iterable, even works for std types
      //
      // \param rng Any iterable
      */
      template <class R>
      constexpr auto operator()(R&& rng) const
          noexcept(noexcept(end(std::forward<R>(rng))))
              -> decltype(end(std::forward<R>(rng))) {
        return end(std::forward<R>(rng));
      }
      //**********************************************************************
    };
    /*! \endcond */
    //************************************************************************

  }  // namespace detail

  // elastica::end is a global function object!
  namespace {
    //**************************************************************************
    /*!\brief The end free function
    // \ingroup utils
    //
    // Marks the end of any iterable, including those of STL types
    //
    // \example
    // Works for STL types
    // \snippet Test_End.cpp end_example
    //
    // And also for \elastica types
    // \snippet Test_End.cpp end_elastica_example TODO!!
    */
    constexpr auto const& end = static_const<detail::impl_end_fn>::value;
    //**************************************************************************
  }  // namespace

}  // namespace elastica

// from_end
namespace elastica {

  //****************************************************************************
  /*!\brief Marks the location of an index from the end
  // \ingroup utils
  */
  struct from_end {
    int i;
  };
  //****************************************************************************

}  // namespace elastica

//==============================================================================
//
//  GLOBAL BINARY ARITHMETIC OPERATOR
//
//==============================================================================

//******************************************************************************
/*!\brief Subtraction operator for marking indices from the end of an iterable
// \ingroup utils
//
// \details
// This operator helps mark the end of an iterable and is useful for slicing.
//
// \param meta The \var end() global function object
// \param i The index from the last location
// \return from_end marking the index from the last location
//
// \example
// \snippet Test_End.cpp end_indexing_example
*/
ELASTICA_ALWAYS_INLINE constexpr elastica::from_end operator-(
    decltype(elastica::end) /* meta */, int i) {
  assert(i >= 0);
  return {i};
}
//******************************************************************************

//******************************************************************************
/*!\brief Subtraction operator for marking indices from the end of an iterable
// \ingroup utils
//
// \details
// This operator helps mark the end of an iterable and is useful for slicing.
//
// \param meta The \var end() global function object
// \param i The index from the last location
// \return from_end marking the index from the last location
//
// \example
// \snippet Test_End.cpp end_indexing_example
*/
ELASTICA_ALWAYS_INLINE constexpr elastica::from_end operator-(
    elastica::from_end end_marker, int i) {
  assert(i >= 0);
  return {end_marker.i + i};
}
//******************************************************************************
