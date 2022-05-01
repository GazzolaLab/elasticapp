//==============================================================================
/*!
//  \file
//  \brief Common policies for batching objects in \elastica modules
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
#include "ErrorHandling/Assert.hpp"

#if ELASTICA_TBB_PARALLEL_MODE
#include <tbb/blocked_range.h>
#endif

namespace elastica {

  namespace parallel {

    //**************************************************************************
    /*!\brief A range of values
     * \ingroup parallel
     *
     * \details
     * blocked_range provides a range of continous numbers, and is modeled after
     * the tbb::range concept.
     *
     * \tparam Value The type of values stored within the range
     */
#if ELASTICA_TBB_PARALLEL_MODE
    template <typename Value>
    using blocked_range = ::tbb::blocked_range<Value>;
#else
    template <typename Value>
    class blocked_range {
     public:
      using size_type = std::size_t;
      using const_iterator = Value;

      //************************************************************************
      /*!\brief Constructor
       */
      blocked_range(Value b, Value e, size_type /*granularity*/ = 1UL)
          : mbegin_(b), mend_(e) {}
      //************************************************************************

      //**Range functions*******************************************************
      /*!\name Range functions*/
      //@{

      //************************************************************************
      /*!\brief Begin value of the range
       */
      const_iterator begin() const { return mbegin_; }
      //************************************************************************

      //************************************************************************
      /*!\brief End value of the range
       */
      const_iterator end() const { return mend_; }
      //************************************************************************

      //************************************************************************
      /*!\brief Size of the range. Undefined behavior if end() < begin().
       */
      size_type size() const noexcept {
        ELASTICA_ASSERT(!(end() < begin()),
                        "size) unspecified if end()<begin()");
        return size_type(mend_ - mbegin_);
      }
      //************************************************************************

      //@}
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! The begin value
      Value mbegin_;
      //! The end value
      Value mend_;
      //************************************************************************
    };
#endif
    //**************************************************************************

    //**************************************************************************
    /*!\brief Idiomatically construct a blocked_range
     * \ingroup parallel
     *
     * \param begin Begin of range
     * \param end End of range
     */
    template <class Value, typename result_t = blocked_range<Value>>
    inline auto make_blocked_range(Value begin, Value end) -> result_t {
      return {std::move(begin), std::move(end)};
    }
    //**************************************************************************

  }  // namespace parallel

}  // namespace elastica
