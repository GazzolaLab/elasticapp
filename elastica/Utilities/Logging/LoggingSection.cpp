//==============================================================================
/*!
//  \file
//  \brief A helper class for logging
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************

#include "Utilities/Logging/LoggingSection.hpp"

#include <iomanip>
#include <string>

#include "ErrorHandling/Assert.hpp"
#include "Utilities/Logging/Logger.hpp"

namespace elastica {

  namespace logging {

    //**************************************************************************
    /*!\brief Level constructor.
     *
     * \details
     * Allows implicit conversion of level to a LoggingSection for use in
     * Singleton Macros
     *
     * \param level The level of logging
     */
    LoggingSection::LoggingSection(LoggingLevel level)
        : logging_level_(level), message_() {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief Destructor
     */
    LoggingSection::~LoggingSection() { commit(); }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Commits the log(message), when finished, to the underlying Logger
     */
    void LoggingSection::commit() {
      // Early exit in case the log message is empty
      if (message_.str().empty())
        return;

      std::string line;
      std::ostringstream oss;

      // Writing the log level information
      switch (logging_level_) {
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
          ELASTICA_ASSERT(false, "Unknown logging level");
          break;
      }

      // Writing the elapsed time
      const time_t elapsed(the_system_clock()->elapsed());
      oss << "[" << std::put_time(std::localtime(&elapsed), "%H:%M:%S") << "] ";

      // Committing the log message
      std::getline(message_, line);
      oss << line << "\n";

      while (std::getline(message_, line)) {
        if (!line.empty())
          oss << "                      " << line << "\n";
      }

      // Logging the formated log message
      std::shared_ptr<Logger> logger(Logger::instance());
      logger->log(oss.str());

      // Resetting the message buffer
      message_.str("");
      message_.clear();
    }
    //**************************************************************************

  }  // namespace logging

}  // namespace elastica
