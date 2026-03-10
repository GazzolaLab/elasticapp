# Distributed under the MIT License.
# See LICENSE.txt for details.

log_info("Finding YamlCpp")

find_package(yaml-cpp CONFIG REQUIRED)

if (yaml-cpp_FOUND)
    log_debug("yaml-cpp version: ${yaml-cpp_VERSION}")
    add_library(YamlCpp INTERFACE IMPORTED)
    target_link_libraries(YamlCpp INTERFACE yaml-cpp::yaml-cpp)
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
