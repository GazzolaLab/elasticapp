#pragma once

#include <fstream>
#ifdef HAVE_TBB
#include <tbb/mutex.h>
#else
#include <mutex>
#endif

#include "Utilities/Requires.hpp"
#include "Utilities/Singleton/Singleton.hpp"
#include "Utilities/SystemClock.hpp"
#include "Utilities/TypeTraits.hpp"

#include "Utilities/Logging/LoggingSection.hpp"

namespace elastica {
  namespace logging {
      /*
       *  Logger singleton class
       *
       *  The Logger class is responsible for writing the
       *  contents of any LoggingSection into the elastic log
       *  file (owned uniquely) by the Logger.
       *
       *  LoggingSections in turn call the logger (using its
       *  instance method)'s log function, which it can access
       *  through friendship.
       */
    class Logger : private Singleton<Logger, SystemClock> {
     private:
      explicit Logger();

     public:
      ~Logger();

     private:
      using StreamType = std::ofstream;
      template <typename Type,  // Type of message with overloaded << operator
                Requires<::tt::is_streamable_v<StreamType, Type>> =
                    nullptr>  // Concept check
      void log(const Type& message);
      void openLogFile();

      // Synchronization mutex for thread-parallel logging.
#ifdef HAVE_TBB
      tbb::mutex _log_mutex;
#else
      std::mutex _log_mutex;
#endif
      // The log file.
      StreamType _log;

      friend class LoggingSection;
      BEFRIEND_SINGLETON
    };
    //*************************************************************************************************

    template <typename Type,  // Type of message with overloaded << operator
              Requires<::tt::is_streamable_v<Logger::StreamType,
                                             Type>>>  // Concept check
    void Logger::log(const Type& message) {
#ifdef HAVE_TBB
      tbb::mutex::scoped_lock lock(_log_mutex);
#else
      std::lock_guard<std::mutex> lock(_log_mutex, std::adopt_lock);
#endif
      if (!_log.is_open())
        openLogFile();
      _log << message;
      _log.flush();
    }

  }  // namespace logging

}  // namespace elastica
