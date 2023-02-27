# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SystemsCommonTraits)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        Protocols.hpp
)

add_subdirectory(DataType)
add_library(${LIBRARY}::DataTraits ALIAS SystemsCommonDataTraits)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        SystemsCommonDataTraits
)
