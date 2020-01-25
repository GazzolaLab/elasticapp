#include "Utilities/Logging/Logger.hpp"

#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>

// TODO : Convert to modern C++ using <chrono>

namespace elastica {
  namespace logging {

    Logger::Logger()
        : Singleton<Logger,
                    SystemClock>(),  // Singleton Initialization, important!
          _log_mutex(),  // Synchronization mutex for thread-parallel logging
          _log()         // The log file
    {}

    Logger::~Logger() {
      if (_log.is_open()) {
        const std::time_t t = theSystemClock()->now();
        std::tm* localTime;
        char c[100];

        // Calculation of the local time
        localTime = std::localtime(&t);

        // Construction of the filename and time string
        std::strftime(c, 100, "%A, %d.%B %Y, %H:%M:%S", localTime);

        // Writing the bottom line
        const size_t length(std::strlen(c));
        _log << "\n--LOG END, " << std::setw(length) << c
             << std::setw(90 - length) << std::setfill('-') << "\n\n"
             << std::endl;

        // Closing the log file
        _log.close();
      }
    }

    void Logger::openLogFile() {
      // Creating the filename
      std::ostringstream filename;
      filename << "elastica";

#if HAVE_TBB
      // TODO Fill this with TBB specific logic, looks not straightforward
      // using https://software.intel.com/en-us/node/506244
      int initialized = TBB_INITiALIZED ? 1 : 0;
      if (initialized) {
        int rank = GET_TBB_RANK();
        filename << rank;
      }
#endif

      filename << ".log";

      // Creating the time and date string for the header line
      const std::time_t t = theSystemClock()->start();
      std::tm* localTime;
      char c[100];

      localTime = std::localtime(&t);
      strftime(c, 100, "%A, %d.%B %Y, %H:%M:%S", localTime);

      // Opening the log file
      _log.open(filename.str().c_str(),
                std::ofstream::out | std::ofstream::trunc);
      if (!_log.is_open()) {
        std::ostringstream oss;
        oss << " Error opening log file '" << filename.str() << "' !\n";
        throw std::runtime_error(oss.str());
      }

      // Writing the header line
      const size_t length(std::strlen(c));
      _log << "\n\n--LOG BEGIN, " << std::setw(length) << c
           << std::setw(88 - length) << std::setfill('-') << "\n\n"
           << std::endl;
    }

  }  // namespace logging
}  // namespace elastica