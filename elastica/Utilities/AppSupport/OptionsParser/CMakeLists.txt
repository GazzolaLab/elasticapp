add_elastica_library(CaseStudyOptionsParser INTERFACE)
elastica_target_headers(CaseStudyOptionsParser
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        PUBLIC
        CaseStudyWithOptions.hpp
        OptionsParser.hpp
        )
target_link_libraries(CaseStudyOptionsParser INTERFACE AppSupportHelpers Utilities Options)
