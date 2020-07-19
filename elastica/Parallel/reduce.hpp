#pragma once

namespace elastica {

  namespace parallel {

    // If serial do
    template <typename Range, typename Body>
    inline void reduce(const Range& range, Body& body) {
      return body(range);
    }

    // TODO If parallel, call TBB

  }  // namespace parallel

}  // namespace elastica