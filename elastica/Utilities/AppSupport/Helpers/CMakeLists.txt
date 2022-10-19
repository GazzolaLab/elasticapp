add_elastica_library(AppSupportHelpers INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        MultipleJustGreaterThan.hpp
        ParamsHolder.hpp
        SplashMessage.hpp
)

target_link_libraries(
        AppSupportHelpers
        INTERFACE
        Utilities
)