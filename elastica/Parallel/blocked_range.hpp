#pragma once

#include "elastica/ErrorHandling/Assert.hpp"

namespace elastica {

  namespace parallel {

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
        ELASTICA_ASSERT(!(end() < begin()), "size) unspecified if end()<begin()");
        return size_type(mend_ - mbegin_);

      }

     private:
      Value mbegin_;
      Value mend_;
    };

  }  // namespace parallel

}  // namespace elastica
