set(LIBRARY ActivationFuncs)

add_elastica_library(${LIBRARY})
## include as "Extension/..."
#target_include_directories(${LIBRARY} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
## include as <elastica/...>
#target_include_directories(${LIBRARY} INTERFACE ${CMAKE_SOURCE_DIR})

set(LIBRARY_SOURCES
        AsActivation.hpp
        Compose.hpp
        Sinusoid.hpp
        InterpolatingSpline.hpp
        Types.hpp
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        ${LIBRARY_SOURCES}
)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        InterpolatingSpline.cpp
)

target_link_libraries(${LIBRARY}
        INTERFACE
        Utilities
        ErrorHandling
        Stl
        PRIVATE
        Spline
        )
