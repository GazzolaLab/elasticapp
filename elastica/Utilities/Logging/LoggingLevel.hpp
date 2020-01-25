#pragma once

namespace elastica {
  namespace logging {

    //*************************************************************************************************
    /*!\brief Logging levels.
     * \ingroup logging
     *
     * The LoggingLevel type enumeration represents the type of the global
     * logging level. It defines all possible levels for the logging
     * functionality. Depending on the setting of the global logging level (see
     * pe::logLevel), more or less information will be written to the log
     * file(s). The following logging levels are available:
     *
     *  - \a inactive: Completely deactivates the logging functionality, i.e.,
     * no log file(s) will be written. Since this setting can immensely
     * complicate error correction, it is not recommended to use this setting!
     *  - \a error   : Only (severe) errors are written to the log file(s).
     *  - \a warning : Extends the \a error setting by warning messages
     * (default).
     *  - \a info    : Extends the \a warning setting by additional informative
     * messages.
     *  - \a progress: Extends the \a info setting by progress information.
     *  - \a debug   : Extends the \a progress setting by debug information.
     *  - \a detail  : Extends the \a debug setting by very fine grained detail
     * information.
     *
     * \a inactive plays a special role in the way that it switches off the
     * logging functionality completely, i.e., no log file(s) will be created.
     * The highest logging level is \a error, which exclusively writes severe
     * errors to the log file(s). The lowest logging level is \a detail, which
     * can create a tremendous amount of logging information. Note that each
     * logging level comprises all higher logging levels. For instance, \a
     * progress will also print all errors and warning to the log file(s).
     */

    enum LoggingLevel {
      inactive = 0,
      error = 1,
      warning = 2,
      info = 3,
      debug = 4
    };
  }  // namespace logging
}  // namespace elastica
