# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Utilities)

add_elastica_library(${LIBRARY})

add_subdirectory(Logging)
add_subdirectory(Math)
add_subdirectory(Singleton)
add_subdirectory(Timing)
add_subdirectory(MemoryMeter)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        SystemClock.cpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        DefineTypes.h
        ForceInline.hpp
        Logging.hpp
        MakeArray.hpp
        MakeSignalingNan.hpp
        Math.hpp
        MemoryMeter.hpp
        MemoryPool.hpp
        NamedType.hpp
        NonCopyable.hpp
        NonCreatable.hpp
        PrintHelpers.hpp
        ProfilingSection.hpp
        Requires.hpp
        Singleton.hpp
        StdHelpers.hpp
        SystemClock.hpp
        TimeStamps.h
        Timing.h
        TMPL.hpp
        TMPLDebugging.hpp
        TypeTraits.hpp
)

target_link_libraries(
        ${LIBRARY}
        PUBLIC
        Blaze
        Brigand
        ErrorHandling
)

#set(LIBRARY_SOURCES Utilities.cpp )
#
#add_elastica_library(${LIBRARY} ${LIBRARY_SOURCES})
#
#target_link_libraries(${LIBRARY} INTERFACE ErrorHandling)
#
