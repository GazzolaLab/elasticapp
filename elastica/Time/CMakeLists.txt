# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Time)

add_elastica_library(${LIBRARY} INTERFACE)

#add_subdirectory(Algebra)
#add_subdirectory(Stepper)
add_subdirectory(SymplecticStepper)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        #        History.hpp
        integrate.hpp
        SimulationTime.hpp
        StepperTags.hpp
        symplectic.hpp
        Tags.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        ErrorHandling
        Utilities
)

