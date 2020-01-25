# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Utilities)

set(LIBRARY_SOURCES Utilities.cpp SystemClock.cpp Logging/Logger.cpp Logging/LoggingSection.cpp)

add_elastica_library(${LIBRARY} ${LIBRARY_SOURCES})

target_link_libraries(${LIBRARY} INTERFACE ErrorHandling)

