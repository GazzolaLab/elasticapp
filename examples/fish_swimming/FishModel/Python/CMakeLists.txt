set(LIBRARY "PyFishModel")

elastica_python_add_module(
        FishModel
        MODULE_PATH "Systems/"
        LIBRARY_NAME ${LIBRARY}
        SOURCES
        BindFishModel.cpp
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
