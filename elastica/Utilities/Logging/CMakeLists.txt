set(LIBRARY Utilities)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        Logger.cpp
        LoggingSection.cpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Logger.hpp
        LoggingLevel.hpp
        LoggingSection.hpp
)
