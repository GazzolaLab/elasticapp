# Distributed under the MIT License. See LICENSE for details.

add_executable(elastica main.cpp)

#target_include_directories(elastica PRIVATE DataStructures)
#target_include_directories(elastica PRIVATE Utilities)
#target_include_directories(elastica PRIVATE Time)
#target_include_directories(elastica PRIVATE Simulator)
#target_include_directories(elastica PRIVATE ErrorHandling)
#target_include_directories(elastica PRIVATE Systems)

#add_subdirectory(DataStructures)
add_subdirectory(ErrorHandling)
add_subdirectory(Utilities)
add_subdirectory(Simulator)
add_subdirectory(Systems)
add_subdirectory(Time)

#target_link_libraries(elastica PUBLIC ${ELASTICA_LIBRARIES})

#set_property(TARGET ElasticaFlags
#        APPEND PROPERTY
#        INTERFACE_COMPILE_OPTIONS
#        $<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>)
#
#check_and_add_cxx_link_flag(-stdlib=libc++)
#check_and_add_cxx_link_flag(-libc++experimental)
#check_and_add_cxx_link_flag(-stdlib=libstdc++)

target_link_libraries(elastica
        PUBLIC
        Simulator
        Environments
        Utilities
        ErrorHandling
        IO
        Systems
        ElasticaFlags
        )
#message(${ElasticaFlags})
#if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
#    message("Bitch")
#    set(CXX_FILESYSTEM_LIBRARIES "stdc++fs")
#    target_link_libraries(elastica PUBLIC ${CXX_FILESYSTEM_LIBRARIES})
#elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
#    set(CXX_FILESYSTEM_LIBRARIES "")
##    set(CXX_FILESYSTEM_LIBRARIES "c++experimental")
#endif ()
#target_link_libraries(elastica PUBLIC ${CXX_FILESYSTEM_LIBRARIES})


