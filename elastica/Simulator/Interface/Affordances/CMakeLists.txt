set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        EmptyBatchingAlgorithm.hpp
        EmptyUpdateSteps.hpp
        EnableReset.hpp
)
