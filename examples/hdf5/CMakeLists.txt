if (TARGET H5)
    set(ELASTICA_EXAMPLE example_hdf5)
    add_elastica_example(${ELASTICA_EXAMPLE} main.cpp)
    target_link_libraries(${ELASTICA_EXAMPLE}
            PUBLIC
            Simulator
            Systems
            Materials
            ElasticaFlags
            H5
            )

    # This not only builds, but also runs a program with display in Cmake.
    # The workaround for this (need to generate a bash file, wrap it in a function, put it in a separate CMake file, and
    # call it with add_custom_target) is needlessly complicated.
    configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/io_hdf5.py"
            "${CMAKE_CURRENT_BINARY_DIR}/io_hdf5.py"
            COPYONLY)

    add_custom_target(
            run_example_hdf5_io
            COMMAND ${ELASTICA_EXAMPLE}
            COMMAND python3 io_hdf5.py
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Checking generated HDF5 file"
            VERBATIM
    )
endif ()
