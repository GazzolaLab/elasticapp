//==============================================================================
/*!
//  \file
//  \brief Functionality surrounding a distinct pair of indices
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

#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>

///
#include "Simulator/Interface/Indexers/Types.hpp"
///
#include "Simulator/Interface/Indexers/TypeTraits.hpp"
#include "Utilities/Defer.hpp"
#include "Utilities/End.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/NumericAt.hpp"
#include "Utilities/Overloader.hpp"
#include "Utilities/TMPL.hpp"

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
      /*!\brief Base type for memoizing a pair of indices
      // \ingroup interface
      */
      struct Pair_base_t : NonCopyable {
        //**Type definitions****************************************************
        //! Type of integral index
        using IntegralIndexType = int;
        //! Type of integral indices from the end
        using EndIntegralIndexType = from_end;
        //! Type of integral indices from the end
        using DeferType = decltype(defer);
        //**********************************************************************

        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief Default constructor.
         */
        Pair_base_t() noexcept = default;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Move constructor.
         */
        Pair_base_t(Pair_base_t&&) noexcept = default;
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~Pair_base_t() = default;
        //@}
        //**********************************************************************

        //**Utility*************************************************************
        /*!\name Utility methods */
        //@{

        //@}
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief A symbolic pair of indices
      // \ingroup interface
      //
      // \details
      // Pair models a pair of indices which one can access. Instead
      // of holding integers in the pair, we hold a symbolic type that
      // can be evaluated by any \elastica system to yield a index. This way we
      // can accommodate dynamically growing systems if the need arises.
      // Additionally it makes the interface much cleaner
      //
      // \tparam FirstType The type of the first index, intended to be an
      // `int`, `end_from` or `defer` type
      // \tparam SecondType The type of the second index, intended to be an
      // `int`, `end_from` or `defer` type
      */
      template <typename FirstType, typename SecondType>
      struct Pair_t : public Pair_base_t {
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief Default constructor.
         */
        constexpr Pair_t(FirstType f, SecondType s) noexcept(
            std::is_nothrow_copy_constructible<FirstType>::value and
                std::is_nothrow_copy_constructible<SecondType>::value)
            : first_index_(f), second_index_(s){};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Move constructor.
         */
        constexpr Pair_t(Pair_t&& other) noexcept
            : first_index_(std::move(other.first_index_)),
              second_index_(std::move(other.second_index_)) {}
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~Pair_t() = default;
        //@}
        //**********************************************************************

        //**Utility*************************************************************
        /*!\name Utility methods */
        //@{

        //**********************************************************************
        /*!\brief Gets the integer indices
         */
        inline constexpr auto first() const noexcept -> FirstType {
          return first_index_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the end integral indices
         */
        inline constexpr auto second() const noexcept -> SecondType {
          return second_index_;
        }
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Member variables****************************************************
        //! The start index of range
        FirstType const first_index_;
        //! The stop index of range
        SecondType const second_index_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief A symbolic pair of indices
      // \ingroup interface
      //
      // \details
      // Pair models a pair of indices which one can access. Instead
      // of holding integers in the pair, we hold a symbolic type that
      // can be evaluated by any \elastica system to yield a index. This way we
      // can accommodate dynamically growing systems if the need arises.
      // Additionally it makes the interface much cleaner
      //
      // \tparam FirstType The type of the first index, intended to be an
      // `int`, `end_from` or `defer` type
      // \tparam SecondType The type of the second index, intended to be an
      // `int`, `end_from` or `defer` type
      */
      template <>
      struct Pair_t<decltype(defer), decltype(defer)> : public Pair_base_t {
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief Default constructor.
         */
        constexpr Pair_t() noexcept = default;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Move constructor.
         */
        constexpr Pair_t(Pair_t&& other) noexcept = default;
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~Pair_t() = default;
        //@}
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
    /*!\brief Idiomatic way to construct a symbolic pair.
    //
    // \details
    // The pair is intended to mark indices in two disjoint sets.
    //
    // \example
    // To construct a range of integers
    // \snippet Test_Pair.cpp pair_example
    // You can also mix end parameters
    // \snippet Test_Pair.cpp pair_end_example
    //
    // \param first_index First index of the pair
    // \param second_index Second index of the pair
    //
    // \note
    // The first_index and second_index types are restricted to either `int` or
    // `end_from` or both are `defer`
    */
    template <typename FirstType, typename SecondType,
              Requires<tt::IsAdmissibleIndex<FirstType>::value> = nullptr,
              Requires<tt::IsAdmissibleIndex<SecondType>::value> = nullptr>
    inline constexpr auto pair(FirstType first_index,
                               SecondType second_index) noexcept
        -> detail::Pair_t<FirstType, SecondType> {
      // if end_from is passed in, we have an overloaded operator for that too
      return detail::Pair_t<FirstType, SecondType>{first_index, second_index};
    }
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Idiomatic way to construct a symbolic pair.
    //
    // \details
    // The pair is intended to mark indices in two disjoint sets.
    //
    // \notes
    // This is a SFINAE specialization for deferred indices
    //
    // \example
    // You can defer the types if you want
    // \snippet Test_Pair.cpp pair_defer_example
    */
    inline constexpr auto pair(decltype(defer), decltype(defer)) noexcept
        -> detail::Pair_t<decltype(defer), decltype(defer)> {
      return detail::Pair_t<decltype(defer), decltype(defer)>{};
    }
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Converts a symbolic pair to a numeric set
    //
    // \details
    // Takes in a pair comprising both integral indices and end indices
    // and converts it to pure integral indices based on the system `sys`. Do
    // not use if the system `sys` is  dynamically growing with time.
    //
    // \example
    // For the system shown below
    // \snippet MockSystems.hpp nothrow_mock_system
    // to_numeric() acts like the following
    // \snippet Test_Pair.cpp pair_to_numeric
    //
    // \note
    // This is a specialization for a compile-time set of integral indices.
    // This routine does too much IMO and can benefit from SRP
    //
    // \param index_pair An index pair constructed using pair()
    // \param first_sys A valid \elastica system that affords index checking
    // with the types `end_from` and `int`
    // \param second_sys A valid \elastica system that affords index checking
    // with the types `end_from` and `int`
    //
    // Both these systems should afford checking with the following syntax
    // \code
        index_check(first_sys, end_from or int) -> int;
       \endcode
    // \exception std::out_of_range if any index within one of the the systems
    // is out of range
    */
    template <typename FirstType, typename SecondType, typename FirstSystem,
              typename SecondSystem>
    auto to_numeric(detail::Pair_t<FirstType, SecondType> const& index_pair,
                    FirstSystem const& first_sys,
                    SecondSystem const& second_sys) -> std::pair<int, int> {
      std::ostringstream s_stream;
      s_stream << "Number of degrees of freedom exceeded at ";
      bool failed = false;

      auto check = make_overloader(
          [&failed, &s_stream](auto const& sys,
                               std::size_t src_idx) -> std::size_t {
            try {
              return index_check(sys, src_idx);
            } catch (const std::out_of_range& exc) {
              s_stream << src_idx;
              failed = true;
              return std::numeric_limits<std::size_t>::max();
            }
          },
          [&failed, &s_stream](auto const& sys,
                               from_end src_idx) -> std::size_t {
            try {
              return index_check(sys, src_idx);
            } catch (const std::out_of_range& exc) {
              s_stream << src_idx.i;
              failed = true;
              return std::numeric_limits<std::size_t>::max();
            }
          });

      s_stream << "(";
      const std::size_t first_idx = check(first_sys, index_pair.first_index_);
      s_stream << ", ";
      const std::size_t second_idx =
          check(second_sys, index_pair.second_index_);
      s_stream << ")";

      if (failed) {
        throw std::out_of_range(s_stream.str());
      }

      return std::make_pair(first_idx, second_idx);
    }
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Converts a symbolic pair to a numeric set
    //
    // \note
    // This is a specialization for deferred type.
    //
    // \param index_pair An index pair constructed using pair() with deferred
    // types
    // \param first_sys A valid \elastica system
    // \param second_sys A valid \elastica system
    */
    template <typename FirstSystem, typename SecondSystem>
    constexpr auto to_numeric(
        detail::Pair_t<decltype(defer), decltype(defer)> const& /* meta */,
        FirstSystem const& /* meta */, SecondSystem const& /* meta */) noexcept
        -> std::pair<decltype(defer), decltype(defer)> {
      return {defer, defer};
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace interface

}  // namespace elastica
