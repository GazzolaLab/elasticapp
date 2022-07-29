set(ELASTICA_EXAMPLE example_profiling)
add_elastica_example(${ELASTICA_EXAMPLE} main.cpp)
target_link_libraries(${ELASTICA_EXAMPLE}
        PUBLIC
        Utilities
        Simulator
        Systems
        Utilities
        Materials
        ElasticaFlags
        )

# This not only builds, but also runs a program with display in Cmake.
# The workaround for this (need to generate a bash file, wrap it in a function, put it in a separate CMake file, and
# call it with add_custom_target) is needlessly complicated.
add_custom_target(
        run_example_profiling_io_csv
        COMMAND ${ELASTICA_EXAMPLE}
        COMMAND python3 io_csv.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Checking generated CSV file"
        VERBATIM
)

if (YamlCpp_FOUND)
    configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/io_yaml.py"
            "${CMAKE_CURRENT_BINARY_DIR}/io_yaml.py"
            COPYONLY)

    add_custom_target(
            run_example_profiling_io_yaml
            COMMAND ${ELASTICA_EXAMPLE}
            COMMAND python3 io_yaml.py
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Checking generated YAML file"
            VERBATIM
    )
else ()
    message(WARNING "YamlCpp not found. Some parts of this example requires yaml support")
endif ()
