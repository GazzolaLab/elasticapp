#include "LoggingSection.hpp"

#include <iomanip>
#include <string>

#include "Logger.hpp"

// TODO : Replace time_t by chrono, too much guesswork :(

namespace elastica {
  namespace logging {

    LoggingSection::LoggingSection(LoggingLevel level)
        : _logging_level(level), _message() {}

    LoggingSection::~LoggingSection() { commit(); }

    void LoggingSection::commit() {
      // Early exit in case the log message is empty
      if (_message.str().empty())
        return;

      std::string line;
      std::ostringstream oss;

      // Writing the log level information
      switch (_logging_level) {
        case LoggingLevel::inactive:
          oss << "[INACTIVE]";
          break;
        case LoggingLevel::error:
          oss << "[ERROR   ]";
          break;
        case LoggingLevel::warning:
          oss << "[WARNING ]";
          break;
        case LoggingLevel::info:
          oss << "[INFO    ]";
          break;
        case LoggingLevel::debug:
          oss << "[DEBUG   ]";
          break;
        default:
          // TODO : Define assert
          // ASSERT(false, "Unknown logging level");
          break;
      }

      // Writing the elapsed time
      const time_t elapsed(theSystemClock()->elapsed());
      std::tm* localTime(std::localtime(&elapsed));
      oss << "[" << std::put_time(localTime, "%H:%M:%S") << "] ";

      // Committing the log message
      std::getline(_message, line);
      oss << line << "\n";

      while (std::getline(_message, line)) {
        if (!line.empty())
          oss << "                      " << line << "\n";
      }

      // Logging the formated log message
      std::shared_ptr<Logger> logger(Logger::instance());
      logger->log(oss.str());

      // Resetting the message buffer
      _message.str("");
      _message.clear();
    }
  }  // namespace logging
}  // namespace elastica
