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

//******************************************************************************
// Includes
//******************************************************************************
#include "ModuleSettings/Parallel.hpp"

#if ELASTICA_TBB_PARALLEL_MODE
#include <tbb/parallel_for.hpp>
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
      template <typename Index, typename F>
      inline void for_each(Index first, Index second, F f) {
        // f(i) => result
        using DummySchedule = std::true_type;
        for_each(blocked_range<Index>(first, second), f, DummySchedule{});
      }

      template <typename Range, typename F, typename Schedule>
      inline void for_each(Range range, F f, Schedule) {
        // std::for_each(range.begin(), range.end(), f);
        const auto start = range.begin();
        const auto end = range.end();
        // quick and dirty
        for (auto idx = start; idx < end; ++idx)
          f(idx);
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
      tbb::parallel_for(std::forward<Args>(args)...);
#else
      ignore_unused<detail::warn_on_disabled_parallelism<> >();
      // we expect a index1, index2, f()
      detail::for_each(std::forward<Args>(args)...);
#endif
    };
    //**************************************************************************

  }  // namespace parallel

}  // namespace elastica