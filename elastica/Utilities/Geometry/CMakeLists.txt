set(LIBRARY Utilities)

#elastica_target_sources(
#        ${LIBRARY}
#        PRIVATE
#        get_closest_line_segment_points.cpp
#        get_closest_point_on_line.cpp
#)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Angles.hpp
#        get_closest_line_segment_points.hpp
#        get_closest_point_on_line.hpp
)
