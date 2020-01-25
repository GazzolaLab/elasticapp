#include "SystemClock.hpp"

namespace elastica {
  time_t SystemClock::start_(std::time(nullptr));
  SystemClock::SystemClock() : Singleton<SystemClock>(){};

}  // namespace elastica
