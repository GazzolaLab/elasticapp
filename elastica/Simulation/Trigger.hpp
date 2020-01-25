#pragma once

#include "Utilities/Policies/NoDelete.hpp"
#include "Utilities/PtrVector.hpp"
#include "Utilities/Singleton.hpp"

namespace elastica {

  // Forward declaration
  struct Time;

  class Trigger {
   private:
    using Triggers = PtrVector<Trigger, NoDelete>;

   public:
    explicit Trigger();
    virtual ~Trigger();

    virtual void operator()(Time t) = 0;
    static inline void trigger_all(Time t);

   private:
    static Triggers _triggers;
  };

  inline void Trigger::trigger_all(Time t) {
    if (_triggers.empty())
      return;

    auto end_triggers = _triggers.end();
    for (Triggers::Iterator trigger = _triggers_.begin();
         trigger != end_triggers; ++trigger) {
      (*trigger)(t);
    }
  }

}  // namespace elastica
