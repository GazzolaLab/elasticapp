#include "Trigger.hpp"

#include <algorithm>

#include "ErrorHandling/Assert.hpp"

namespace elastica {

  //=================================================================================================
  //
  //  DEFINITION AND INITIALIZATION OF THE STATIC MEMBER VARIABLES
  //
  //=================================================================================================

  // Static initialization, https://stackoverflow.com/a/185848
  Trigger::Triggers Trigger::triggers_;

  //=================================================================================================
  //
  //  CONSTRUCTOR
  //
  //=================================================================================================
  /*!\brief Constructor for the Trigger class.
   */
  Trigger::Trigger() { triggers_.push_back(this); }

  //=================================================================================================
  //
  //  DESTRUCTOR
  //
  //=================================================================================================

  //*************************************************************************************************
  /*!\brief Destructor for the Trigger class.
   */

  Trigger::~Trigger() {
    auto it(std::find(triggers_.begin(), triggers_.end(), this));
    ELASTICA_ASSERT(it != triggers_.end(),
                    "Cannot find trigger object in registry!");
    triggers_.erase(it);
  }
  //*************************************************************************************************

}  // namespace elastica