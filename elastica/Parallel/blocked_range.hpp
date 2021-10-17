#pragma once

#include "ErrorHandling/Assert.hpp"

#if ELASTICA_TBB_PARALLEL_MODE
#include <tbb/blocked_range.h>
#endif

namespace elastica {

  namespace parallel {

#if ELASTICA_TBB_PARALLEL_MODE
    template <typename Value>
    using blocked_range = ::tbb::blocked_range<Value>;
#else
    template <typename Value>
    class blocked_range {
     public:
      using size_type = std::size_t;
      using const_iterator = Value;

      blocked_range(Value b, Value e, size_type /*granularity*/ = 1UL)
          : mbegin_(b), mend_(e) {}

      const_iterator begin() const { return mbegin_; }
      const_iterator end() const { return mend_; }
      //! Size of the range
      /** Unspecified if end()<begin(). */
      size_type size() const noexcept {
        ELASTICA_ASSERT(!(end() < begin()),
                        "size) unspecified if end()<begin()");
        return size_type(mend_ - mbegin_);
      }

     private:
      Value mbegin_;
      Value mend_;
    };
#endif

  }  // namespace parallel

}  // namespace elastica
