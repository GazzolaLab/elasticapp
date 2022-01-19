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
#include <cstdint>  // for size_t
#include <utility>

#include "ModuleSettings/Parallel.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/MakeString.hpp"

#if ELASTICA_TBB_PARALLEL_MODE
#include <tbb/global_control.h>
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
        const std::size_t n_active_threads = tbb::global_control::active_value(
            tbb::global_control::max_allowed_parallelism);
        if (n_active_threads != n_threads) {
          auto warning_message = std::string(
              MakeString{}
              << "elastica++ was initialized with " << n_threads
              << " threads, but the number of threads set by the "
                 "user in the application level scope is "
              << n_active_threads
              << "threads. This behavior is fine if you have other "
                 "libraries running alongside elastica++. In case this is "
                 "not the behavior that you wanted, before initializing the "
                 "simulator please insert the following code to set the "
                 "number of threads for the entire application :\n"
              << "const int n_threads_used_by_application = /* number of "
                 "threads needed*/; \n"
                 "tbb::global_control "
                 "global_limit(tbb::global_control::max_allowed_parallelism,"
                 "n_threads_used_by_application);\n");
          logging::log<logging::warning>([msg = std::move(warning_message)](
                                             auto const& log) { log << msg; });
        }
#else
        ignore_unused(n_threads);
#endif
      }

    }  // namespace detail

  }  // namespace parallel

}  // namespace elastica
