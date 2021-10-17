//==============================================================================
/*!
//  \file
//  \brief Parallelism details
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
#include "Utilities/IgnoreUnused.hpp"

#if ELASTICA_TBB_PARALLEL_MODE
#include <tbb/task_scheduler_init.h>
#endif

namespace elastica {

  namespace parallel {

    namespace detail {

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <bool B = true>
      struct [[deprecated(R"warn(
You requested a parallelized configuration but disabled parallelism during
CMake generation/compilation step!
)warn")]] warn_on_disabled_parallelism{};
      /*! \endcond */
      //************************************************************************

      inline void initialize_smp(const std::size_t n_threads) {
#if ELASTICA_TBB_PARALLEL_MODE
        static tbb::task_scheduler_init* init = nullptr;
        init = new tbb::task_scheduler_init(n_threads);
#else
        ignore_unused(n_threads);
#endif
      }

    }  // namespace detail

  }  // namespace parallel

}  // namespace elastica
