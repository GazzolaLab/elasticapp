# Distributed under the MIT License.
# See LICENSE.txt for details.

log_info("Finding YamlCpp")

find_package(YamlCpp REQUIRED)

log_debug("YamlCpp_LIBRARIES: ${YamlCpp_LIBRARIES}")
log_debug("YamlCpp_INCLUDE_DIRS: ${YamlCpp_INCLUDE_DIRS}")

add_library(YamlCpp INTERFACE IMPORTED)
# Have to use `set_property` before CMake version 3.11. See:
# https://gitlab.kitware.com/cmake/cmake/-/merge_requests/1264
set_property(TARGET YamlCpp
        APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${YamlCpp_INCLUDE_DIRS})
set_property(TARGET YamlCpp
        APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${YamlCpp_LIBRARIES})

add_interface_lib_headers(
        TARGET YamlCpp
        HEADERS
        yaml-cpp/yaml.h
)

set_property(
        GLOBAL APPEND PROPERTY ELASTICA_THIRD_PARTY_LIBS
        YamlCpp
)

add_library(_ElasticaUseYAMLCpp INTERFACE)

if (YamlCpp_FOUND)
    set_property(
            TARGET _ElasticaUseYAMLCpp
            APPEND PROPERTY
            INTERFACE_COMPILE_DEFINITIONS
            $<$<COMPILE_LANGUAGE:CXX>:ELASTICA_USE_YAMLCPP>
    )
else ()
    message(WARNING "YamlCpp not found. All YAML related functionalities are turned off.")
endif ()

target_link_libraries(
        ElasticaFlags
        INTERFACE
        _ElasticaUseYAMLCpp
)
