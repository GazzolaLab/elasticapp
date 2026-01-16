//==============================================================================
/*!
//  \file
//  \brief Functionality surrounding a set of indices
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

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>

///
#include "Simulator/Interface/Indexers/Types.hpp"
///
#include "Simulator/Interface/Indexers/TypeTraits.hpp"
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
      /*!\brief Base type for memoizing a compact set of indices
      // \ingroup interface
      */
      struct At_base_t : NonCopyable {
        //**Type definitions****************************************************
        //! Type of integral index
        using IntegralIndexType = std::size_t;
        //! Type of integral indices from the end
        using EndIntegralIndexType = from_end;
        //**********************************************************************

        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief Default constructor.
         */
        At_base_t() noexcept = default;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Move constructor.
         */
        At_base_t(At_base_t&&) noexcept = default;
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        virtual ~At_base_t() = default;
        //@}
        //**********************************************************************

        //**Utility*************************************************************
        /*!\name Utility methods */
        //@{

        //**********************************************************************
        /*!\brief Gets the integer indices
         */
        virtual IntegralIndexType* integer_indices() noexcept = 0;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the number of integer indices
         */
        virtual std::uint8_t integer_indices_size() const noexcept = 0;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the end integral indices
         */
        virtual EndIntegralIndexType* end_indices() noexcept = 0;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the number of end indices
         */
        virtual std::uint8_t end_indices_size() const noexcept = 0;
        //**********************************************************************

        //@}
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      // while one can use variant, there's no reason for runtime penalty
      // here

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief A compact set of indices known at compile-time
      // \ingroup interface
      */
      template <std::uint8_t Nints, std::uint8_t Nends>
      struct At_t : public At_base_t {
        //**Type definitions****************************************************
        //! Type of array for integral indices
        using IntegralIndexArray = std::array<IntegralIndexType, Nints>;
        //! Type of array for integral indices from the end
        using EndIntegralIndexArray = std::array<EndIntegralIndexType, Nends>;
        //! Aggregate type for all integral indices
        using Indices = std::pair<IntegralIndexArray, EndIntegralIndexArray>;
        //**********************************************************************

        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief Index constructor
        //
        // \param indices A pair of integral and end index arrays
        */
        constexpr explicit At_t(Indices&& indices) noexcept
            : indices_(std::move(indices)){};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Array constructor.
        //
        // \param i array of integer indices
        // \param e array of integer indices from the end
        //
        // \note
        // Why is the constructor of a pair not noexcept?
        */
        constexpr At_t(IntegralIndexArray i, EndIntegralIndexArray e)
            : indices_(i, e){};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Move constructor.
         */
        At_t(At_t&& other) noexcept : indices_(std::move(other.indices_)) {}
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        ~At_t() override = default;
        //@}
        //**********************************************************************

        // mostly unused
        //**Utility*************************************************************
        /*!\name Utility methods */
        //@{

        //**********************************************************************
        /*!\brief Gets the integer indices
         */
        IntegralIndexType* integer_indices() noexcept override {
          return std::get<IntegralIndexArray>(indices_).data();
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the number of integer indices
         */
        std::uint8_t integer_indices_size() const noexcept override {
          return Nints;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the end integral indices
         */
        EndIntegralIndexType* end_indices() noexcept override {
          return std::get<EndIntegralIndexArray>(indices_).data();
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the number of end indices
         */
        std::uint8_t end_indices_size() const noexcept override {
          return Nends;
        }
        //**********************************************************************

        // Additional utility functions
        //**********************************************************************
        /*!\brief Gets the integer indices retaining strong array type
        //
        // \return mutable reference to the integer array
        */
        constexpr inline auto ints() & noexcept -> IntegralIndexArray& {
          return std::get<IntegralIndexArray>(indices_);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the integer indices retaining strong array type
        //
        // \return const reference to the integer array
        */
        constexpr inline auto ints() const& noexcept
            -> IntegralIndexArray const& {
          return std::get<IntegralIndexArray>(indices_);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the end integer indices retaining strong array type
        //
        // \return mutable reference to the end integer array
        */
        constexpr inline auto ends() & noexcept -> EndIntegralIndexArray& {
          return std::get<EndIntegralIndexArray>(indices_);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the end integer indices retaining strong array type
        //
        // \return const reference to the end integer array
        */
        constexpr inline auto ends() const& noexcept
            -> EndIntegralIndexArray const& {
          return std::get<EndIntegralIndexArray>(indices_);
        }
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Member variables****************************************************
        //! The indices of interest
        Indices indices_;
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
    /*!\brief Constructs a compact set of indices from index parameters
    //
    // \example
    // \snippet Test_At.cpp at_example
    //
    // \note
    // This is a specialization for integral indices
    //
    // \param x An integral/end-integral index
    // \param ...indices Other integral/end-integral indices
    */
    template <typename T, typename... Indices,
              Requires<std::conjunction_v<
                  tt::IsAdmissibleIndex<std::decay_t<T>>,
                  tt::IsAdmissibleIndex<std::decay_t<Indices>>...>> = nullptr>
    constexpr auto at(T&& x, Indices&&... indices) noexcept {
      using Args = tmpl::list<std::decay_t<T>, std::decay_t<Indices>...>;

      // count ends first
      constexpr std::uint8_t n_ends =
          tmpl::count_if<Args, std::is_same<tmpl::_1, from_end>>::value;
      // count indices
      constexpr std::uint8_t n_indices = tmpl::size<Args>::value - n_ends;

      // separately construct arrays from them
      using AtType = detail::At_t<n_indices, n_ends>;
      using IntegralIndexArray = typename AtType::IntegralIndexArray;
      using EndIntegralIndexArray = typename AtType::EndIntegralIndexArray;
      IntegralIndexArray ia{};
      EndIntegralIndexArray ea{};

      // then initialize
      // we can make a compile-time evaluated function , but it seems overkill
      auto f = make_overloader(
          [idx = 0UL, &ia](typename AtType::IntegralIndexType i) mutable {
            ia[idx++] = i;
          },
          [idx = 0UL, &ea](typename AtType::EndIntegralIndexType e) mutable {
            ea[idx++] = e;
          });
      {
        f(x);
        EXPAND_PACK_LEFT_TO_RIGHT(f(indices));
      }
      return AtType{std::make_pair(ia, ea)};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Constructs a range of indices from a range
    //
    // \example
    // \snippet Test_At.cpp at_range_example
    //
    // \note
    // This is a specialization for a range type just returns the same range
    // back out.  SFINAE is the only way to achieve this with forwarding
    // references.
    //
    // \param range An index range
    */
    template <typename T,
              Requires<tt::IsRange<std::decay_t<T>>::value> = nullptr>
    constexpr decltype(auto) at(T&& range) noexcept {
      return std::forward<T>(range);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Constructs a pair of indices from a pair
    //
    // \example
    // \snippet Test_At.cpp at_pair_example
    //
    // \note
    // This is a specialization for a pair type which just returns the same pair
    // out. SFINAE is the only way to achieve this with forwarding references.
    //
    // \param pair An index pair
    // \return pair
    */
    template <typename T,
              Requires<tt::IsPair<std::decay_t<T>>::value> = nullptr>
    constexpr decltype(auto) at(T&& pair) noexcept {
      return std::forward<T>(pair);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Converts a symbolic set of indices to a numeric set
    //
    // \details
    // Takes in an index set comprising both of integral indices and end indices
    // and converts it to pure integral indices based on the system `sys`. Do
    // not use if the system `sys` is  dynamically growing with time
    //
    // \example
    // For the system shown below
    // \snippet MockSystems.hpp nothrow_mock_system
    // to_numeric() acts like the following
    // \snippet Test_At.cpp to_numeric_example
    //
    // \note
    // This is a specialization for a compile-time set of integral indices.
    // This routine does too much IMO and can benefit from SRP
    //
    // \param index_set An index set constructed using at()
    // \param sys A valid \elastica system that affords index checking with
    // the types `end_from` and `int` using the syntax
    // \code
        index_check(sys, end_from or int) -> int;
    // \endcode
    // \exception std::out_of_range if any index within the system is out of sys
    // range
    */
    template <std::uint8_t Nints, std::uint8_t Nends, typename System>
    auto to_numeric(detail::At_t<Nints, Nends> const& index_set,
                    System const& sys)
        -> ::elastica::detail::numeric_at_t<Nints + Nends> {
      // there may be overflow here, but its less likely
      std::array<std::size_t, Nints + Nends> numerical_indices{};
      std::ostringstream s_stream;
      s_stream << "Number of degrees of freedom exceeded at ";

      using AtType = detail::At_t<Nints, Nends>;
      bool failed = false;
      auto check = make_overloader(
          [&failed, &s_stream,
           &sys](typename AtType::IntegralIndexType src_idx) mutable
          -> std::size_t {
            try {
              return index_check(sys, src_idx);
            } catch (const std::out_of_range& exc) {
              s_stream << src_idx << ", ";
              failed = true;
              return std::numeric_limits<std::size_t>::max();
            }
          },
          [&failed, &s_stream,
           &sys](typename AtType::EndIntegralIndexType src_idx) mutable
          -> std::size_t {
            try {
              return index_check(sys, src_idx);
            } catch (const std::out_of_range& exc) {
              s_stream << src_idx.i << ", ";
              failed = true;
              return std::numeric_limits<std::size_t>::max();
            }
          });

      s_stream << "Integer indices: [ ";
      index_apply<Nints>(
          [&check, &numerical_indices, &ints = index_set.ints()](auto... Is) {
            // Is : 0 -> Nints
            EXPAND_PACK_LEFT_TO_RIGHT(std::get<Is>(numerical_indices) =
                                          check(std::get<Is>(ints)));
          });
      s_stream << " ], End indices: [ ";
      index_apply<Nends>(
          [&check, &numerical_indices, &ends = index_set.ends()](auto... Is) {
            EXPAND_PACK_LEFT_TO_RIGHT(std::get<Nints + Is>(numerical_indices) =
                                          check(std::get<Is>(ends)));
          });
      s_stream << "]";

      if (failed) {
        throw std::out_of_range(s_stream.str());
      }
      return ::elastica::detail::numeric_at_t<Nints + Nends>{numerical_indices};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Converts a symbolic set of indices to a numeric set
    //
    // \details
    // Takes in an index set comprising both of integral indices and end indices
    // and converts it to pure integral indices based on the system `sys`. Do
    // not use if the system `sys` is  dynamically growing with time
    //
    // \example
    // For the system shown below
    // \snippet MockSystems.hpp nothrow_mock_system
    // to_numeric() acts like the following
    // \snippet Test_At.cpp to_numeric_runtime_example
    //
    // \note
    // This is a specialization for a run-time set of integral indices.
    // This routine does too much IMO and can benefit from SRP
    //
    // \param index_set An index set constructed using at()
    // \param sys A valid \elastica system that affords index checking with
    // the types `end_from` and `int` using the syntax
    // \code
        index_check(sys, end_from or int) -> int;
    // \endcode
    // \exception std::out_of_range if any index within the system is out of sys
    // range
    */
    template <typename System>
    auto to_numeric(std::unique_ptr<detail::At_base_t>&& index_set,
                    System const& sys)
        -> std::unique_ptr<::elastica::detail::numeric_at_base_t> {
      std::ostringstream s_stream;
      s_stream << "Number of degrees of freedom exceeded at ";
      bool failed = false;
      using AtType = detail::At_base_t;

      // drop int vector
      std::vector<std::size_t> numerical_indices;
      numerical_indices.reserve(index_set->integer_indices_size() +
                                index_set->end_indices_size());

      auto check = make_overloader(
          [&failed, &s_stream,
           &sys](typename AtType::IntegralIndexType src_idx) mutable -> int {
            try {
              return index_check(sys, src_idx);
            } catch (const std::out_of_range& exc) {
              s_stream << src_idx << ", ";
              failed = true;
              return std::numeric_limits<int>::max();
            }
          },
          [&failed, &s_stream,
           &sys](typename AtType::EndIntegralIndexType src_idx) mutable -> int {
            try {
              return index_check(sys, src_idx);
            } catch (const std::out_of_range& exc) {
              s_stream << src_idx.i << ", ";
              failed = true;
              return std::numeric_limits<int>::max();
            }
          });

      // reserve
      s_stream << "Integer indices: [ ";
      std::transform(
          index_set->integer_indices(),
          index_set->integer_indices() + index_set->integer_indices_size(),
          std::back_inserter(numerical_indices), check);
      s_stream << " ], End indices: [ ";
      // do ends next
      std::transform(index_set->end_indices(),
                     index_set->end_indices() + index_set->end_indices_size(),
                     std::back_inserter(numerical_indices), check);
      s_stream << "]";

      if (failed) {
        throw std::out_of_range(s_stream.str());
      }

      return std::unique_ptr<::elastica::detail::numeric_at_base_t>(
          new ::elastica::detail::runtime_numeric_at_t(
              std::move(numerical_indices)));
    }
    //**************************************************************************

  }  // namespace interface

}  // namespace elastica
