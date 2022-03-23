//==============================================================================
/*!
//  \file
//  \brief parallel for_each
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

// Since this header only wraps tbb and modifications to it we mark
// it as a system header file so that clang-tidy ignores it.
#ifdef __GNUC__
#pragma GCC system_header
#endif

//******************************************************************************
// Includes
//******************************************************************************
#include <algorithm>

#include "ModuleSettings/Parallel.hpp"

#if ELASTICA_TBB_PARALLEL_MODE
#include <tbb/parallel_for_each.h>
#endif

#include "Parallel/blocked_range.hpp"
#include "Parallel/detail.hpp"
#include "Utilities/IgnoreUnused.hpp"

namespace elastica {

  namespace parallel {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      // have overloads for tbb as possible
      template <typename Iterator, typename F>
      inline void for_each(Iterator first, Iterator second, F f) {
        std::for_each(first, second, f);
      }

      template <typename Container, typename F>
      inline void for_each(Container& container, F&& f) {
        // needs to be qualified, else ADL kicks in and all stl is dragged in
        ::elastica::parallel::detail::for_each(
            std::begin(container), std::end(container), std::forward<F>(f));
      }
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

    //**************************************************************************
    /* \brief Calls function f for all items from [first, last) interval
     * \ingroup parallel
     *
     * \param funcs functions to execute in parallel
     */
    template <typename... Args>
    inline void for_each(Args&&... args) {
#if ELASTICA_TBB_PARALLEL_MODE
      tbb::parallel_for_each(std::forward<Args>(args)...);
#else
      ignore_unused<detail::warn_on_disabled_parallelism<> >();
      // we expect a index1, index2, f()
      detail::for_each(std::forward<Args>(args)...);
#endif
    };
    //**************************************************************************

  }  // namespace parallel

}  // namespace elastica