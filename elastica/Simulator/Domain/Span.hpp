//==============================================================================
/*!
//  \file
//  \brief Span implementation
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

#include <cstddef>

#include "ErrorHandling/Assert.hpp"
#include "Simulator/Domain/Types.hpp"

namespace elastica {

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  /*!\brief Class template representing a span of physical length
  // \ingroup domain
  //
  // The Span class template represents a physical span extending up to the
  // domain in one-dimension. It stores the values of the minimum and maximum
  // position for this dimension.
  //
  // \note Its implementation is similar to std::array and semantically, it also
  // resides on the stack.
  //
  // \tparam Numeric Any numeral type
  */
  template <typename Numeric>  // Numeric
  struct Span {
   public:
    //**Static members**********************************************************
    //! Minimum index accessible in this span
    static constexpr std::size_t min_idx = 0UL;

    //! Maxmimum index accessible in this span
    static constexpr std::size_t max_idx = 1UL;
    //**************************************************************************

    //**Constructors************************************************************
    /*!\name Constructors */
    //@{
    explicit Span(Numeric arg_min, Numeric arg_max);
    Span(const Span& other) = default;
    //@}
    //**************************************************************************

    //**Destructor**************************************************************
    /*!\name Destructor */
    //@{
    ~Span() = default;
    //@}
    //**************************************************************************

    //**Assignment operators****************************************************
    /*!\name Assignment operators */
    //@{
    inline Span& operator=(const Span& rhs) & = default;
    //@}
    //**************************************************************************

    //**Member variables********************************************************
    Numeric span_[2UL];  //!< Storage of spans
    //**************************************************************************
  };
  //****************************************************************************

  //============================================================================
  //
  //  CONSTRUCTORS
  //
  //============================================================================

  //**Constructor***************************************************************
  /*!\brief Constructor for the Group class.
  //
  // \param arg_min Minimum value of the span
  // \param arg_max Maximum value of the span
  */
  template <typename Numeric>
  Span<Numeric>::Span(Numeric arg_min, Numeric arg_max) {
    // TODO : No assert in constructor
    ELASTICA_ASSERT(
        arg_min < arg_max,
        "Wrong span of simulation, please provide min first and max second!");
    span_[min_idx] = arg_min;
    span_[max_idx] = arg_max;
  }
  //****************************************************************************

}  // namespace elastica