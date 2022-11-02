# Distributed under the MIT License.
# See LICENSE.txt for details.
set(LIBRARY "PyMagnetizedCosseratRods")

elastica_python_add_module(
        MagnetizedCosseratRods
        MODULE_PATH "Systems/"
        LIBRARY_NAME ${LIBRARY}
        SOURCES
        Bindings.cpp
        BindMagnetizedCosseratRod.cpp
        BindMagnetizedCosseratRodWithoutDamping.cpp
)

elastica_python_link_libraries(
        ${LIBRARY}
        PRIVATE
        MagnetismExtension
        pybind11::module
        PyGeneratorsCosseratRod
        Blaze
)

elastica_python_add_dependencies(${LIBRARY} PyArrays PyTags)
