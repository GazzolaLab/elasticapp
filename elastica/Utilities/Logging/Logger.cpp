//==============================================================================
/*!
//  \file
//  \brief The implemention of a logger class, managing all logging resources
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

//******************************************************************************
// Includes
//******************************************************************************

#include "Utilities/Logging/Logger.hpp"

#include <iomanip>
#include <sstream>

namespace elastica {

  namespace logging {

    //**************************************************************************
    /*!\brief Default constructor.
     */
    Logger::Logger()
        : SingletonType(),  // initialize the singleton, important!
          _log_mutex(),     // Synchronization mutex for thread-parallel logging
          _log()            // The log file
    {}
    //**************************************************************************

    //**************************************************************************
    /*!\brief Destructor
     */
    Logger::~Logger() {
      if (_log.is_open()) {
        // Writing the header line
        pre_post("LOG END");

        // Closing the log file
        _log.close();
      }
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Affixes prefix and postfix in the log file
     *
     * \param opt optional character for pre/post-fixing
     */
    void Logger::pre_post(const char* const opt = "") noexcept {
      const auto t = the_system_clock()->now();
      const char* const pre_postfix = {"--"};
      _log << "\n\n"
           << pre_postfix << opt
           << ", "
           // Creating the time and date string for the header line
           << std::put_time(std::localtime(&t), "%c") << pre_postfix << "\n\n"
           << std::endl;
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Opens the underlying log file for logging
     *
     * \throws std::ios_base::failure
     */
    void Logger::open_logfile() {
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

      // Opening the log file
      _log.open(filename.str().c_str(),
                std::ofstream::out | std::ofstream::trunc);
      if (not _log.is_open()) {
        std::ostringstream oss;
        oss << " Error opening log file '" << filename.str() << "' !\n";
        throw std::ios_base::failure(oss.str());
      }
      // Writing the header line
      pre_post("LOG BEGIN");
    }
    //**************************************************************************

  }  // namespace logging

}  // namespace elastica