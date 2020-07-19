set(LIBRARY Time)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        SymplecticStepperKernel.hpp
        SymplecticStepperTags.hpp
        SymplecticSystemCollectionStepper.hpp
        SymplecticSystemStepper.hpp
        Algorithms/PositionVerlet.hpp
        Algorithms/VelocityVerlet.hpp
)
