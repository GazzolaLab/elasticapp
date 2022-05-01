# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CosseratRodBlazeOperations)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        OpsTraits.hpp
        OptimizationLevel.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        OpsTraits.hpp
        OptimizationLevel.hpp
)

add_subdirectory(InvRotateDivide)
add_library(${LIBRARY}::InvRotateDivide ALIAS InvRotateDivideKernel)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
        Blaze
        BlazeTensor
        InvRotateDivideKernel
)

