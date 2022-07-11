# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SimulatorUtilities)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        MemoryMeters.hpp
        Offsets.hpp
        Timers.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        MemoryMeters.hpp
        Offsets.hpp
        Timers.hpp
)

target_link_libraries(${LIBRARY} INTERFACE Utilities)
