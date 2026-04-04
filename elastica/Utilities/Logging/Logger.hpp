//==============================================================================
/*!
//  \file
//  \brief The implemention of a logger class, managing all logging resources
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <fstream>
#ifdef HAVE_TBB
#include <tbb/mutex.h>
#else
#include <mutex>
#endif

///
#include "Utilities/Logging/Types.hpp"
///
#include "Utilities/Singleton/Singleton.hpp"
#include "Utilities/SystemClock.hpp"

namespace elastica {

  namespace logging {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Logging facilities
     * \ingroup logging
     *
     * \details
     * Logger is responsible for writing the contents of any LoggingSection
     * into the elastic log file (owned and managed solely) by the Logger.
     * LoggingSection then in turns call into the logger (using its instance
     * method) having filled the message with sufficient context.
     *, which it can access
     * through friendship.
     *
     * \see log(), LoggingSection
     */
    class Logger : private Singleton<Logger, SystemClock> {
     private:
      //**Friendships***********************************************************
      //! Friends for the singleton pattern
      BEFRIEND_SINGLETON;
      //! Friends for the logging facility
      friend class LoggingSection;
      //************************************************************************

     protected:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      explicit Logger();
      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      ~Logger();
      //@}
      //************************************************************************

     private:
      //**Type definitions******************************************************
      //! Type of singleton
      using Singleton<Logger, SystemClock>::SingletonType;
      //! Type of the stream to be logged in
      using StreamType = std::ofstream;
      //! Type of the mutex for the stream type
#ifdef HAVE_TBB
      using MutexType = tbb::mutex;
#else
      using MutexType = std::mutex;
#endif
      //************************************************************************

      //**Utility functions*****************************************************
      /*!\name Utility functions */
      //@{

      //************************************************************************
      /*!\brief Logs a string message to the underlying streams
       *
       * \param message Message to log
       *
       * \see log_to_stream(), tee()
       */
      void log(std::string const& message) {
#ifdef HAVE_TBB
        tbb::mutex::scoped_lock lock(_log_mutex);
#else
        // scoped lock from C++17 onwards
        std::lock_guard<std::mutex> lock(_log_mutex);
#endif
        if (not _log.is_open())
          open_logfile();
        _log << message;
        _log.flush();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Opens the underlying log file for logging
       */
      void open_logfile();
      //************************************************************************

      //************************************************************************
      /*!\brief Affixes prefix and postfix in the log file
       *
       * \param opt optional character for pre/post-fixing
       */
      void pre_post(const char* const /* opt */) noexcept;
      //************************************************************************

      //@}
      //************************************************************************

      //**Member variables******************************************************
      //! Synchronization mutex for thread-parallel logging.
      MutexType _log_mutex;
      //! The log file.
      StreamType _log;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace logging

}  // namespace elastica
