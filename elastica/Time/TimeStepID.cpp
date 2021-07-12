//
// Created by Tejaswin Parthasarathy on 7/10/21.
//

#include "Time/TimeStepID.hpp"

#include <ostream>

namespace elastica {

  bool operator==(const TimeStepID& a, const TimeStepID& b) noexcept {
    return a.step() == b.step() and a.substep() == b.substep();
  }

  bool operator!=(const TimeStepID& a, const TimeStepID& b) noexcept {
    return not(a == b);
  }

  bool operator<(const TimeStepID& a, const TimeStepID& b) noexcept {
    return (a.step() < b.step()) or
           (a.step() == b.step() and a.substep() < b.substep());
  }
  bool operator<=(const TimeStepID& a, const TimeStepID& b) noexcept {
    return not(b < a);
  }
  bool operator>(const TimeStepID& a, const TimeStepID& b) noexcept {
    return b < a;
  }
  bool operator>=(const TimeStepID& a, const TimeStepID& b) noexcept {
    return not(a < b);
  }

  std::ostream& operator<<(std::ostream& s, const TimeStepID& id) noexcept {
    return s << id.step() << ':' << id.substep();
  }
}  // namespace elastica