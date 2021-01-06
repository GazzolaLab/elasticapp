# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY Options)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Factory.hpp
        Options.hpp
        OptionsDetails.hpp
        Parameter.hpp
        ParseOptions.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        ErrorHandling
        Utilities
        YamlCpp
)
