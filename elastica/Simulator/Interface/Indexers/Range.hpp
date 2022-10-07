//==============================================================================
/*!
//  \file
//  \brief A symbolic range of indices
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
#include <limits>
#include <sstream>

///
#include "Simulator/Interface/Indexers/Types.hpp"
///
#include "Simulator/Interface/Indexers/TypeTraits.hpp"
#include "Utilities/End.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/NumericRange.hpp"
#include "Utilities/Overloader.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace elastica {

  namespace interface {

    namespace detail {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief A symbolic range of indices
      // \ingroup interface
      //
      // \details
      // Range models a range of indices on which one can iterate upon. Instead
      // of holding integer limits to the range, we hold a symbolic type that
      // can be evaluated by any \elastica system to yield a range. This way we
      // can accommodate dynamically growing systems if the need arises.
      // Additionally it makes the interface much cleaner
      //
      // \tparam StartType The start type of the range, intended to be an
      // `int` or `end_from` type
      // \tparam StopType The stop type of the range, intended to be an
      // `int` or `end_from` type
      */
      template <typename StartType, typename StopType>
      struct Range : public NonCopyable {
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief Default constructor.
         */
        constexpr Range(StartType st, StopType so, int se) noexcept(
            std::is_nothrow_copy_constructible<StartType>::value and
                std::is_nothrow_copy_constructible<StopType>::value)
            : start_(st), stop_(so), step_(se){};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Move constructor.
         */
        constexpr Range(Range&& other) noexcept
            : start_(std::move(other.start_)),
              stop_(std::move(other.stop_)),
              step_(other.step_) {}
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~Range() = default;
        //@}
        //**********************************************************************

        //**Member variables****************************************************
        //! The start index of range
        StartType const start_;
        //! The stop index of range
        StopType const stop_;
        //! The integer step of range
        int const step_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

    //==========================================================================
    //
    //  UTILITY FUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Idiomatic way to construct a half-open symbolic range.
    //
    // \details
    // The range is closed from left and open from right and models
    // \f$ [a, b) \f$, where \f$ a \f$ is the start and \f$ b \f$ is the stop.
    //
    // \example
    // To construct a range of integers
    // \snippet Test_Range.cpp range_example
    // You can also mix end parameters
    // \snippet Test_Range.cpp range_end_example
    //
    // \param start Start of the range
    // \param stop Stop of the range
    // \param step Step of the range
    //
    // \note
    // The start and stop types are restricted to either `int` or `end_from`
    */
    template <typename StartType, typename StopType,
              Requires<tt::IsAdmissibleIndex<StartType>::value> = nullptr,
              typename ActualStopType = decltype(std::declval<StopType>() -
                                                 std::declval<int>()),
              Requires<tt::IsAdmissibleIndex<ActualStopType>::value> = nullptr>
    inline constexpr auto range(StartType start, StopType stop,
                                int step = 1) noexcept
        -> detail::Range<StartType, ActualStopType> {
      // stop - 0 here in case people pass in end, which then converts to
      // end_from
      // if end_from is passed in, we have an overloaded operator for that too
      return detail::Range<StartType, ActualStopType>{start, stop - 0, step};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Converts a symbolic range to a numeric set
    //
    // \details
    // Takes in a range comprising both integral indices and end indices
    // and converts it to pure integral indices based on the system `sys`. Do
    // not use if the system `sys` is  dynamically growing with time
    //
    // \example
    // For the system shown below
    // \snippet MockSystems.hpp nothrow_mock_system
    // to_numeric() acts like the following
    // \snippet Test_Range.cpp range_to_numeric
    //
    // \note
    // This is a specialization for a compile-time set of integral indices.
    // This routine does too much IMO and can benefit from SRP
    //
    // \param index_range An index set constructed using range()
    // \param sys A valid \elastica system that affords index checking with
    // the types `end_from` and `int` using the syntax
    // \code
        index_check(sys, end_from or int) -> int;
       \endcode
    // \exception std::out_of_range if any index within the system is out of sys
    // range
    */
    template <typename StartType, typename StopType, typename System>
    auto to_numeric(detail::Range<StartType, StopType> const& index_range,
                    System const& sys) -> ::elastica::detail::NumericRange {
      std::ostringstream s_stream;
      s_stream << "Number of degrees of freedom exceeded at range ";
      bool failed = false;

      auto check = make_overloader(
          [&failed, &s_stream, &sys](std::size_t src_idx) -> std::size_t {
            try {
              return index_check(sys, src_idx);
            } catch (const std::out_of_range& exc) {
              s_stream << src_idx;
              failed = true;
              return std::numeric_limits<std::size_t>::max();
            }
          },
          [&failed, &s_stream, &sys](from_end src_idx) -> std::size_t {
            try {
              return index_check(sys, src_idx);
            } catch (const std::out_of_range& exc) {
              s_stream << src_idx.i;
              failed = true;
              return std::numeric_limits<std::size_t>::max();
            }
          });

      s_stream << "[ start: ";
      const std::size_t start = check(index_range.start_);
      // workaround to support ranges with last index : stop_ - 1 does not
      // throw in check(), and once we have the numeric index, we add one to
      // recover original index.
      s_stream << ", stop: ";
      const std::size_t stop = check(index_range.stop_ - 1UL) + 1UL;
      s_stream << "]";

      if (failed) {
        throw std::out_of_range(s_stream.str());
      }

      return numeric_range(start, stop, index_range.step_);
    }
    //**************************************************************************

  }  // namespace interface

}  // namespace elastica