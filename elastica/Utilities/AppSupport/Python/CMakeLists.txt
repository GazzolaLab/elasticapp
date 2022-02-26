# Distributed under the MIT License.
# See LICENSE.txt for details.

if (${ELASTICA_BUILD_PYTHON_BINDINGS})
    set(LIBRARY "PyBindings")

    add_elastica_library(${LIBRARY} INTERFACE)

    elastica_target_headers(
            ${LIBRARY}
            INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica/Utilities/AppSupport/Python
            HEADERS
            BoundChecks.hpp
    )

    target_link_libraries(${LIBRARY}
            INTERFACE
            Utilities)
endif ()

if (${ELASTICA_BUILD_PYTHON_BINDINGS})
    set(LIBRARY "PyBlaze")

    elastica_python_add_module(
            Blaze
            LIBRARY_NAME ${LIBRARY}
            SOURCES
            Bindings.cpp
            BlazeVector.cpp
            BlazeSubvector.cpp
            BlazeMatrix.cpp
            BlazeSubmatrix.cpp
            BlazeTensor.cpp
            BlazeSubtensor.cpp
    )

    elastica_python_link_libraries(
            ${LIBRARY}
            PRIVATE
            Blaze
            BlazeTensor
            pybind11::module
            Utilities
    )
endif ()
