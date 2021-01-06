# Distributed under the MIT License.
# See LICENSE.txt for details.

log_info("Finding YamlCpp")

find_package(YAMLCPP REQUIRED)

log_debug("YAMLCPP_LIBRARIES: " ${YAMLCPP_LIBRARIES})
log_debug("YAMLCPP_INCLUDE_DIRS: " ${YAMLCPP_INCLUDE_DIRS})

add_library(YamlCpp INTERFACE IMPORTED)
# Have to use `set_property` before CMake version 3.11. See:
# https://gitlab.kitware.com/cmake/cmake/-/merge_requests/1264
set_property(TARGET YamlCpp
        APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${YAMLCPP_INCLUDE_DIRS})
set_property(TARGET YamlCpp
        APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${YAMLCPP_LIBRARIES})

add_interface_lib_headers(
        TARGET YamlCpp
        HEADERS
        yaml-cpp/yaml.h
)

set_property(
        GLOBAL APPEND PROPERTY ELASTICA_THIRD_PARTY_LIBS
        YamlCpp
)
