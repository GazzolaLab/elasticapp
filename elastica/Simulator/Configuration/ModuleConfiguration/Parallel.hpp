//
// Created by Tejaswin Parthasarathy on 10/13/21.
//

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include <cstddef>  // std::size_t
#include <cstdlib>  // for getenv

#include "Simulator/Configuration/ModuleConfiguration/Types.hpp"
//==============================================================================
//
//  CONFIGURATION
//
//==============================================================================
#include "ModuleSettings/Parallel.hpp"  // for threading hint

namespace elastica {

  namespace configuration {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Configuration for parallelism
     * \ingroup config
     *
     * \details
     * ConstraintsConfiguration denotes the compile-time customization point
     * of the degree of parallelism in elastica
     *
     * \see make_parallel_configuration() for idiomatic construction
     */
    class ParallelConfiguration {
     public:
      constexpr ParallelConfiguration(int user_hint,
                                      std::size_t env_hint) noexcept
          : user_threads_hint(user_hint), environment_threads_hint(env_hint) {}
      constexpr ParallelConfiguration() = delete;
      constexpr ParallelConfiguration(ParallelConfiguration const&) noexcept =
          default;
      constexpr ParallelConfiguration(ParallelConfiguration&&) noexcept =
          default;

      //************************************************************************
      /*!\brief Obtains the number of threads
       */
      inline auto constexpr n_threads() const noexcept -> std::size_t {
        return user_threads_hint > 0
                   ? static_cast<std::size_t>(user_threads_hint)
                   : environment_threads_hint;
      }
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! Hint for number of threads from the user
      int const user_threads_hint;
      //! Hint for number of threads from the environment
      std::size_t const environment_threads_hint;
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes a parallel configuration
     * \ingroup config
     *
     * \param user_threads_hint Number of threads to initialize the elastica
     * engine with
     *
     * \example
     * \snippet ModuleConfiguration/Test_Parallel.cpp eg
     *
     * \see ParallelConfiguration
     */
    inline auto make_parallel_configuration(const int user_threads_hint = -1)
        -> ParallelConfiguration {
      // can throw, hence outside
      const std::size_t env_threads_hint = []() {
        const char* cnthreads = std::getenv(ENV_ELASTICA_NUM_THREADS);
        if (cnthreads)
          return static_cast<std::size_t>(std::atoi(cnthreads));  // NOLINT
        else
          // compile_time_hint
          return elastica_threads_hint();
      }();
      using RT = ParallelConfiguration;
      return RT{user_threads_hint, env_threads_hint};
    }
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica