# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Time)

add_elastica_library(${LIBRARY} INTERFACE)

#add_subdirectory(Algebra)
#add_subdirectory(Stepper)
add_subdirectory(Helpers)
add_subdirectory(SymplecticSteppers)
add_subdirectory(Protocols)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        #        History.hpp
        Helpers.hpp
        integrate.hpp
        SimulationTime.hpp
        StepperTags.hpp
        SymplecticSteppers.hpp
        Tags.hpp
        Types.hpp
        TypeTraits.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        ErrorHandling
        Utilities
)

