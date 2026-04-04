//==============================================================================
/*!
//  \file
//  \brief Trigger interface for per-step simulation callbacks
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

#include <vector>

//#include "Utilities/Policies/NoDelete.hpp"
//#include "Utilities/PtrVector.hpp"
#include "Utilities/Singleton.hpp"

// TODO Rework the entire Trigger framework to eliminate static calls
//      We can take information directly from the simulator object which contains
//      information rather than a registrar
namespace elastica {

  // Forward declaration
//  struct Time;

  /*!
   * \brief Base class for trigger objects that receive simulation progress
   * updates
   *
   * Trigger instances register themselves in a static registry and are invoked
   * through trigger_all(). This mechanism is currently global in scope and is
   * intended as a lightweight hook for step-based side effects.
   */
  class Trigger {
   private:
//    using Triggers = PtrVector<Trigger, NoDelete>;
    using Triggers = std::vector<Trigger*>;

   public:
    explicit Trigger();
    virtual ~Trigger();

    //! Respond to the current simulation time and step.
    virtual void trigger(double time_v, std::size_t simulation_step) = 0;
    //! Invoke all registered triggers for the given time and step.
    static inline void trigger_all(double time_v, std::size_t simulation_step);

   private:
    static Triggers triggers_;
  };

  inline void Trigger::trigger_all(double time_v, std::size_t simulation_step) {
    if (triggers_.empty())
      return;

    for (auto t : triggers_){
      t->trigger(time_v, simulation_step);
    }
  }

}  // namespace elastica
