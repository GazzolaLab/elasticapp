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
  // position for this dimension. Its implementation is similar to std::array
  // and semantically, it is meant to reside on the stack.
  */
  template <typename Real>  // Real
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
    explicit Span(Real arg_min, Real arg_max);
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
    Real span_[2UL];  //!< Storage of spans
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
  template <typename Real>
  Span<Real>::Span(Real arg_min, Real arg_max) {
    // TODO : No assert in constructor
    ELASTICA_ASSERT(
        arg_min < arg_max,
        "Wrong span of simulation, please provide min first and max second!");
    span_[min_idx] = arg_min;
    span_[max_idx] = arg_max;
  }
  //****************************************************************************

}  // namespace elastica