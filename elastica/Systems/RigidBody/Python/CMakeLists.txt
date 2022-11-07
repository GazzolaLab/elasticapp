# Distributed under the MIT License.
# See LICENSE.txt for details.

add_subdirectory(Generators)

set(LIBRARY "PyRigidBody")

elastica_python_add_module(
        RigidBody
        MODULE_PATH "Systems/"
        LIBRARY_NAME ${LIBRARY}
        SOURCES
        Bindings.cpp
        BindSphere.cpp
)

elastica_python_link_libraries(
        ${LIBRARY}
        PRIVATE
        RigidBody
        pybind11::module
        Blaze
        PyGeneratorsRigidBody
)

elastica_python_add_dependencies(${LIBRARY} PyArrays PyTags)
