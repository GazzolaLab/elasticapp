# Distributed under the MIT License.
# See LICENSE.txt for details.

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
