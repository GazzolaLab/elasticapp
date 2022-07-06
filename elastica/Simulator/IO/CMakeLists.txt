# Distributed under the MIT License. See LICENSE.txt for details.

# TODO Refactor IO into a separate module depending on the Systems module (not Simulator)
set(LIBRARY IO)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(Serialization)
add_library(${LIBRARY}::Serialization ALIAS Serialization)

#elastica_target_sources(
#        ${LIBRARY}
#        PRIVATE
#        vtk/Writer.cpp
#        csv/Writer.cpp
#)
#
elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        #        vtk.hpp
        #        vtk/Writer.hpp
        #        csv/Writer.hpp
        Serialization.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Simulator # For Triggers and visualization
        Serialization
        # PRIVATE
        std::filesystem
        #        -stdlib=libc++
        #        stdc++fs
)

