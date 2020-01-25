#pragma once

#include "Utilities/Logging/LoggingLevel.hpp"
#include "Utilities/Logging/LoggingSection.hpp"

namespace elastica {
  namespace logging {

    constexpr LoggingLevel loglevel = error;

#define LOG_INACTIVE_SECTION(NAME)               \
  if (elastica::logging::loglevel >=             \
      elastica::logging::LoggingLevel::inactive) \
    if (elastica::logging::LoggingSection NAME =     \
            elastica::logging::LoggingLevel::inactive)

#define LOG_ERROR_SECTION(NAME)                                              \
  if (elastica::logging::loglevel >= elastica::logging::LoggingLevel::error) \
    if (elastica::logging::LoggingSection NAME =                                 \
            elastica::logging::LoggingLevel::error)

#define LOG_WARNING_SECTION(NAME)                                              \
  if (elastica::logging::loglevel >= elastica::logging::LoggingLevel::warning) \
    if (elastica::logging::LoggingSection NAME =                                   \
            elastica::logging::LoggingLevel::warning)

#define LOG_INFO_SECTION(NAME)                                              \
  if (elastica::logging::loglevel >= elastica::logging::LoggingLevel::info) \
    if (elastica::logging::LoggingSection NAME =                                \
            elastica::logging::LoggingLevel::info)

#define LOG_DEBUG_SECTION(NAME)                                              \
  if (elastica::logging::loglevel >= elastica::logging::LoggingLevel::debug) \
    if (elastica::logging::LoggingSection NAME =                                 \
            elastica::logging::LoggingLevel::debug)

  }  // namespace logging
}  // namespace elastica
