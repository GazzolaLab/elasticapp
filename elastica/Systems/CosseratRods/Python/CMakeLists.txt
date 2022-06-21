# Distributed under the MIT License.
# See LICENSE.txt for details.

add_subdirectory(Generators)

set(LIBRARY "PyCosseratRods")

elastica_python_add_module(
        CosseratRods
        MODULE_PATH "Systems/"
        LIBRARY_NAME ${LIBRARY}
        SOURCES
        Bindings.cpp
        BindCosseratRod.cpp
        BindCosseratRodWithoutDamping.cpp
)

elastica_python_link_libraries(
        ${LIBRARY}
        PRIVATE
        CosseratRods
        pybind11::module
        PyGeneratorsCosseratRod
        Blaze
)

elastica_python_add_dependencies(${LIBRARY} PyArrays PyTags)
