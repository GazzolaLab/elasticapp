//==============================================================================
/*!
//  \file
//  \brief A global functor for get
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
#include <utility>

#include "Systems/Block.hpp"
#include "Utilities/StaticConst.hpp"
#include "Utilities/TaggedTuple.hpp"

namespace elastica {

  namespace detail {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================
    using ::blocks::get;
    using ::std::get;
    using ::tuples::get;

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Get type
     * \ingroup utils
     *
     * \details
     * Type of the global get functor for a collection whose size is known at
     * compile time
     */
    template <typename T>
    struct impl_get_fn {
      //************************************************************************
      /*!\brief Get the members of a tuple like type
       *
       * \param rng Any iterable
       */
      template <class R>
      constexpr auto operator()(R&& rng) const
          noexcept(noexcept(get<T>(std::forward<R>(rng))))
              -> decltype(get<T>(std::forward<R>(rng))) {
        return get<T>(std::forward<R>(rng));
      }
      //************************************************************************
    };
    /*! \endcond */
    //************************************************************************

  }  // namespace detail

  // elastica::get is a global function object!
  namespace {

    //**************************************************************************
    /*!\brief The get free function
     * \ingroup utils
     *
     * Gets members from any collection whose size is known at compile-time,
     * including those of STL types
     *
     * \example
     * Works for STL types
     * \snippet Test_Get.cpp get_tuple_example
     * \snippet Test_Get.cpp get_pair_example
     *
     * And also for \elastica types
     * \snippet Test_Get.cpp get_tagged_tuple_example
     */
    template <typename T>
    constexpr auto const& get = static_const<detail::impl_get_fn<T>>::value;
    //**************************************************************************

  }  // namespace

}  // namespace elastica
