//==============================================================================
/*!
//  \file
//  \brief Invoke a function in parallel over a sequence of elements, reducing
//  the result into a single value.
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************
#include "ModuleSettings/Parallel.hpp"

#if ELASTICA_TBB_PARALLEL_MODE
#include <tbb/parallel_reduce.h>
#endif

#include <utility>

#include "Parallel/detail.hpp"

namespace elastica {

  namespace parallel {

    //**************************************************************************
    /*!\brief Applies a reduce operator in parallel
     * \ingroup parallel
     *
     * \details
     * parallel_reduce(range,body) performs parallel reduction of body over each
     * value in range.
     *
     * \example
     * \code
     *  struct Sum {
     *    float value;
     *    Sum() : value(0) {}
     *    Sum( Sum& s, split ) {value = 0;}
     *    void operator()(const elastica::parallel::blocked_range<float*>& r) {
     *        float temp = value;
     *        for( float* a=r.begin(); a!=r.end(); ++a ) {
     *            temp += *a;
     *        }
     *        value = temp;
     *    }
     *    void join( Sum& rhs ) {value += rhs.value;}
     * };
     *
     * float ParallelSum( float array[], size_t n ) {
     *    Sum total;
     *    elastica::parallel::reduce(
     *      elastica::parallel::make_blocked_range( array, array+n ),
     *      total );
     *    return total.value;
     * }
     * \endcode
     *
     * \param range Blocked range to reduce over
     * \param body Body of parallel reduction
     * \param args Additional arguments that is passed to the parallelism engine
     */
    template <typename Range, typename Body, typename... Args>
    inline void reduce(const Range& range, Body& body, Args&&... args) {
#if ELASTICA_TBB_PARALLEL_MODE
      tbb::parallel_reduce(range, body, std::forward<Args>(args)...);
#else
      ignore_unused<detail::warn_on_disabled_parallelism<> >();
      ignore_unused(std::forward<Args>(args)...);
      return body(range);
#endif
    }
    //**************************************************************************

  }  // namespace parallel

}  // namespace elastica
