# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Frames)
add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        EulerianFrame.hpp
        LagrangianFrame.hpp
)

target_link_libraries(${LIBRARY} INTERFACE Utilities)
