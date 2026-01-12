//==============================================================================
/*!
//  \file
//  \brief Marks the level of logging
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include "Configuration/Logging.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace elastica {

  namespace logging {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Logging levels.
     * \ingroup logging
     *
     * The LoggingLevel type enumeration represents the type of the global
     * logging level. It defines all possible levels for the logging
     * functionality. Depending on the setting of the global logging level (see
     * elastica::log_level), more or less information will be written to the log
     * file(s). The following logging levels are available:
     *
     *  - `inactive`: Completely deactivates the logging functionality, i.e.,
     *                no log file(s) will be written. Since this setting can
     *                immensely complicate error correction, it is not
     *                recommended to use this setting!
     *  - `error`   : Only (severe) errors are written to the log file(s).
     *  - `warning` : Extends the `error` setting by warning messages (default).
     *  - `info`    : Extends the `warning` setting by additional informative
     *                messages.
     *  - `debug`   : Extends the `info` setting by debug information.
     *
     * `inactive` plays a special role in the way that it switches off the
     * logging functionality completely, i.e., no log file(s) will be created.
     * The highest logging level is `error`, which exclusively writes severe
     * errors to the log file(s). The lowest logging level is `debug`, which
     * can create a tremendous amount of logging information. Note that each
     * logging level comprises all higher logging levels. For instance,
     * `info` will also print all errors and warning to the log file(s).
     */
    enum LoggingLevel : unsigned short {
      inactive = 0,  // least
      error,
      warning,
      info,
      debug  // highest
    };
    //**************************************************************************

    //==========================================================================
    //
    //  METAFUNCTIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief The log level selected by the user set to ELASTICA_LOG_LEVEL()
     *
     * \see ELASTICA_LOG_LEVEL()
     */
    constexpr LoggingLevel log_level = ELASTICA_LOG_LEVEL;
    //**************************************************************************

    //**************************************************************************
    /*!\brief Compile-time check for determining if a level should be logged
     *
     * \tparam LoggingLevel level of logging
     * \metareturns std::bool_constant
     */
    template <LoggingLevel L>
    inline constexpr auto to_be_logged() {
      return std::bool_constant<(log_level >= L)>{};
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Compile-time check for determining if a level should not be logged
     *
     * \tparam LoggingLevel level of logging
     * \metareturns std::bool_constant
     *
     * \see to_be_logged()
     */
    template <LoggingLevel L>
    inline constexpr auto to_not_be_logged() {
      return std::negation<decltype(to_be_logged<L>())>{};
    }
    //**************************************************************************

  }  // namespace logging

}  // namespace elastica
