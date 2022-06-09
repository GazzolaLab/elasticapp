# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "PyTags")

elastica_python_add_module(
        Tags
        MODULE_PATH "Systems/"
        LIBRARY_NAME ${LIBRARY}
        SOURCES
        Bindings.cpp
        BindTags.cpp
)

elastica_python_link_libraries(
        ${LIBRARY}
        PRIVATE
        Systems
        pybind11::module
)