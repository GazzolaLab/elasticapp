#include <algorithm>

#include "ErrorHandling/Assert.hpp"
#include "Trigger.hpp"

namespace elastica {

  // Static initialization, https://stackoverflow.com/a/185848
  static Trigger::Triggers Trigger::_triggers;

  explicit Trigger::Trigger() { _triggers.push_back(this); }

  ~Trigger::Trigger(){
    Triggers::Iterator it = std::find(_triggers.begin(), _triggers.end(), this);
    ASSERT(it != _triggers.end(), "Cannot find trigger object in registry!");
    _triggers.erase(this);
  };
}  // namespace elastica