# Distributed under the MIT License. See LICENSE.txt for details.

if (TARGET HighFive)
    set(LIBRARY H5)
    add_elastica_library(${LIBRARY} INTERFACE)
    target_compile_definitions(${LIBRARY} INTERFACE ELASTICA_USE_H5)

    add_subdirectory(Interface)
    add_library(${LIBRARY}::Interface ALIAS H5Interface)

    add_subdirectory(STL)
    add_library(${LIBRARY}::Stl ALIAS H5Stl)

    add_subdirectory(Time)
    add_library(${LIBRARY}::Time ALIAS H5Time)

    add_subdirectory(Utilities)
    add_library(${LIBRARY}::Utilities ALIAS H5Utilities)

    add_subdirectory(Systems)
    add_library(${LIBRARY}::Systems ALIAS H5Systems)

    set(LIBRARY_SOURCES
            _Impl.hpp
            _Interface.hpp
            STL.hpp
            Systems.hpp
            Time.hpp
            Types.hpp
            Utilities.hpp
            )

    elastica_target_headers(
            ${LIBRARY}
            INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
            ${LIBRARY_SOURCES}
    )

    elastica_target_sources(
            ${LIBRARY}
            INTERFACE
            ${LIBRARY_SOURCES}
    )

    target_link_libraries(
            ${LIBRARY}
            INTERFACE
            H5Interface
            H5Stl
            H5Systems
            H5Time
            H5Utilities
    )
else ()
    message(WARNING "HighFive not found. All HDF5 related functionalities are turned off.")
endif ()

