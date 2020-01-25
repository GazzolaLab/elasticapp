# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY ErrorHandling)

set(LIBRARY_SOURCES AbortWithErrorMessage.cpp FloatingPointExceptions.cpp)

add_elastica_library(${LIBRARY} ${LIBRARY_SOURCES})
