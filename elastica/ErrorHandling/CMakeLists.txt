# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY ErrorHandling)

add_elastica_library(${LIBRARY})

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        AbortWithErrorMessage.cpp
        FloatingPointExceptions.cpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        AbortWithErrorMessage.hpp
        Abort.hpp
        Assert.hpp
        Error.hpp
        ExpectsAndEnsures.hpp
        Exit.hpp
        FloatingPointExceptions.hpp
)

#add_elastica_library(${LIBRARY} ${LIBRARY_SOURCES})
