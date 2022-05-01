//==============================================================================
/*!
//  \file
//  \brief Invoke a collection of functions in parallel
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
#include "ModuleSettings/Parallel.hpp"

#if ELASTICA_TBB_PARALLEL_MODE
#include <tbb/parallel_invoke.h>
#endif

#include "Parallel/detail.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace parallel {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename... F>
      inline void invoke(F&&... funcs) {
        EXPAND_PACK_LEFT_TO_RIGHT(funcs());
      }
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

    //**************************************************************************
    /*!\brief Executes a list of functions in parallel
     * \ingroup parallel
     *
     * \details
     * The functions are prescribed as parallel tasks to a runtime engine if
     * \elastica is complied with requested parallelism. The function blocks
     * until for all tasks are completed.
     *
     * \example
     * \code
     * auto f1 = [i = 0]() mutable { i *= 2; };
     * auto f2 = [j = 1]() mutable { j -= 1; };
     * auto f3 = [x = 2]() mutable { x += 2; };
     * elastica::parallel::invoke(f1, f2, f3);
     * \endcode
     *
     * \param funcs functions to execute in parallel
     */
    template <typename... F>
    inline void invoke(F&&... funcs) {
#if ELASTICA_TBB_PARALLEL_MODE
      tbb::parallel_invoke(std::forward<F>(funcs)...);
#else
      ignore_unused<detail::warn_on_disabled_parallelism<> >();
      // we expect a index1, index2, f()
      detail::invoke(std::forward<F>(funcs)...);
#endif
    };
    //**************************************************************************

  }  // namespace parallel

}  // namespace elastica
