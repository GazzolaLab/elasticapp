set(LIBRARY Utilities)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        AxisAngleRotation.cpp
        GetClosestLineSegmentPoints.cpp
        GetClosestPointOnLine.cpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Angles.hpp
        AxisAngleRotation.hpp
        GetClosestLineSegmentPoints.hpp
        GetClosestPointOnLine.hpp
)
