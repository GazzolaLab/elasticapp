#pragma once
#include "Time/Algebra/Operations.hpp"

template <class StateType>
struct operations_dispatcher {
  using operations_type = default_operations;
};
