//==============================================================================
/*!
//  \file
//  \brief A numeric range of indices
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

#include "Utilities/NonCopyable.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TypeTraits/IsA.hpp"

namespace elastica {

  namespace detail {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief A numeric range of indices
    // \ingroup utils
    //
    // \details
    // Range models a range of indices on which one can iterate upon or evaluate
    // in case of floating point ranges.
    //
    // \tparam F The arithmetic type of the range
    */
    template <typename F>
    struct NumericRangeImpl : public NonCopyable {
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{

      //************************************************************************
      /*!\brief Default constructor.
       */
      constexpr NumericRangeImpl(F st, F so, F se) noexcept(
          std::is_nothrow_move_constructible<F>::value)
          : start_(std::move(st)), stop_(std::move(so)), step_(std::move(se)){};
      //************************************************************************

      //************************************************************************
      /*!\brief Move constructor.
       */
      constexpr NumericRangeImpl(NumericRangeImpl&& other) noexcept(
          std::is_nothrow_move_constructible<F>::value)
          : start_(std::move(other.start_)),
            stop_(std::move(other.stop_)),
            step_(std::move(other.step_)) {}
      //************************************************************************

      //@}
      //************************************************************************

      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~NumericRangeImpl() = default;
      //@}
      //************************************************************************

      //**Member variables******************************************************
      // makes little sense to modify a range after creation, hence declared
      // const
      //! The start index of range
      F const start_;
      //! The stop index of range
      F const stop_;
      //! The step of range
      F const step_;
      //************************************************************************
    };
    /*! \endcond */
    //**************************************************************************

    //==========================================================================
    //
    //  ALIAS DECLARATIONS
    //
    //==========================================================================

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Alias declaration for an integer NumericRangeImpl.
    // \ingroup utils
    */
    using NumericRange = NumericRangeImpl<int>;
    /*! \endcond */
    //**************************************************************************

  }  // namespace detail

  namespace tt {
    //**************************************************************************
    /*!\brief Checks whether a type models \elastica NumericRange
    // \ingroup utils type_traits
    //
    // \example
    // \snippet Test_NumericRange.cpp is_numeric_range_example
    */
    template <typename T>
    struct IsNumericRange : ::tt::is_a<detail::NumericRangeImpl, T> {};
    //**************************************************************************
  }  // namespace tt

  //============================================================================
  //
  //  UTILITY FUNCTIONS
  //
  //============================================================================

  //****************************************************************************
  /*!\brief Idiomatic way to construct a half-open numeric range
  //
  // \details
  // The range is closed from left and open from right and models
  // \f$ [a, b) \f$, where \f$ a \f$ is the start and \f$ b \f$ is the stop.
  //
  // \example
  // \snippet Test_NumericRange.cpp numeric_range_example
  //
  // \param start Start of the range
  // \param stop Stop of the range
  // \param step Step of the range
  //
  // \note
  // - while the name is more verbose, it differentiates itself from interface::
  // range().
  // - The parameters are restricted to C++ arithmetic types
  */
  template <typename F, Requires<std::is_arithmetic<F>::value> = nullptr>
  inline constexpr auto make_numeric_range(
      F start, F stop,
      F step = 1) noexcept(noexcept(detail::NumericRangeImpl<F>{
      start, stop, step})) -> detail::NumericRangeImpl<F> {
    return detail::NumericRangeImpl<F>{start, stop, step};
  }
  //****************************************************************************

  //  template <typename F>
  //  auto sanitize(detail::NumericRangeImpl<F> const& index_range, std::size_t
  //  n_dofs) noexcept
  //  -> detail::NumericRangeImpl<F>{
  //    // there may be overflow here, but its less likely
  //    auto bound = [nd = static_cast<int>(n_dofs)](int x) -> int {
  //      return x < 0 ? (nd + x) : x;
  //    };
  //    return index_apply<N>([&bound, &args = index_set.args](auto... Is) {
  //      return at(bound(std::get<Is>(args))...);
  //    });
  //  }
}  // namespace elastica
