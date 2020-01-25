#pragma once

#include <ctime>
#include <memory>

#include "Utilities/Singleton/Singleton.hpp"

namespace elastica {

  class SystemClock : private Singleton<SystemClock> {
   private:
    explicit SystemClock();

   public:
    ~SystemClock() = default;
    inline time_t start() const;
    inline time_t now() const;
    inline time_t elapsed() const;

   private:
    static time_t start_;  //!< Timestamp for the start of the process.
    friend std::shared_ptr<SystemClock> theSystemClock();
    BEFRIEND_SINGLETON
  };

  inline std::shared_ptr<SystemClock> theSystemClock() {
    return SystemClock::instance();
  }

  inline time_t SystemClock::start() const { return start_; }

  inline time_t SystemClock::now() const { return time(nullptr); }

  inline time_t SystemClock::elapsed() const {
    return std::time(nullptr) - start_;
  }

}  // namespace elastica
