//==============================================================================
/*!
//  \file
//  \brief Span implementation
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

#include <array>
#include <cstddef>

#include "Simulator/Domain/Types.hpp"

namespace elastica {

  //============================================================================
  //
  //  FORWARD DECLARATIONS
  //
  //============================================================================

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <typename Numeric>
  struct Span;

  template <typename Numeric>
  inline auto make_span(Numeric arg_min, Numeric arg_max) noexcept
      -> Span<Numeric>;
  /*! \endcond */
  //****************************************************************************

  //============================================================================
  //
  //  CLASS DEFINITION
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Class template representing a span of physical length
   * \ingroup domain
   *
   * The Span class template represents a physical span extending up to the
   * domain in one-dimension. It stores the values of the minimum and maximum
   * position for this dimension.
   *
   * \note Its implementation is similar to std::array and semantically, it also
   * resides on the stack.
   *
   * \tparam Numeric Any numeral type
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
   protected:
    //**************************************************************************
    /*!\brief Constructor for the Span class.
     *
     * \param arg_min Minimum value of the span
     * \param arg_max Maximum value of the span
     */
    explicit Span(Numeric arg_min, Numeric arg_max) noexcept
        : span_{arg_min, arg_max} {};
    //**************************************************************************

   public:
    //! Copy constructor (defaulted).
    Span(const Span& other) = default;
    //@}
    //**************************************************************************

    //**Friendships*************************************************************
    /*! \cond ELASTICA_INTERNAL */
    //! Friendship with make_span
    friend auto make_span<Numeric>(Numeric, Numeric) noexcept -> Span<Numeric>;
    /*! \endcond */
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

    //**Subscript operator******************************************************
    /*!\name Subscript operator */
    //@{
    inline auto operator[](std::size_t idx) & noexcept -> Numeric& {
      return span_[idx];
    };
    inline auto operator[](std::size_t idx) const& noexcept -> Numeric const& {
      return span_[idx];
    };
    //@}
    //**************************************************************************

    //**Member variables********************************************************
    //! The storage type of all spans
    using Storage = std::array<Numeric, 2UL>;
    //!< Storage of spans
    Storage span_;
    //**************************************************************************
  };
  //****************************************************************************

  //****************************************************************************
  /*!\brief Makes a span idiomatically
   *
   * \example
   * \snippet Test_Span.cpp make_span_eg
   *
   * \param arg_min (expected) minimum value of the span
   * \param arg_max (expected) maximum value of the span
   * \return Span
   *
   * \note
   * The arguments may be switched depending on whether arg_min < arg_max
   */
  template <typename Numeric>
  inline auto make_span(Numeric arg_min, Numeric arg_max) noexcept
      -> Span<Numeric> {
    using RT = Span<Numeric>;
    return (arg_min < arg_max) ? RT(arg_min, arg_max) : RT(arg_max, arg_min);
  }
  //****************************************************************************

}  // namespace elastica