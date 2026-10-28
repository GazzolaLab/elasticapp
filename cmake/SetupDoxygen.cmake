message(STATUS "Finding Doxygen")
find_package(Doxygen REQUIRED)

set(ELASTICA_DOCS_OUTPUT_DIR "${PROJECT_BINARY_DIR}/docs/html")
set(ELASTICA_DOCS_DOXYFILE "${PROJECT_BINARY_DIR}/docs/Doxyfile")
set(ELASTICA_DOCS_THEME_DIR "${PROJECT_BINARY_DIR}/docs/doxygen-awesome-css")
set(ELASTICA_DOCS_WARN_LOG "${PROJECT_BINARY_DIR}/docs/doxygen-warnings.log")
set(ELASTICA_DOCS_TARGET_OUTPUT_ROOT "${PROJECT_BINARY_DIR}")

set(ELASTICA_DOXYGEN_HTML_EXTRA_STYLESHEETS "")
set(ELASTICA_DOXYGEN_HTML_EXTRA_FILES "")

include(FetchContent)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
FetchContent_Declare(
        doxygen-awesome-css
        URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
        )
FetchContent_MakeAvailable(doxygen-awesome-css)
FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

string(JOIN " \\\n    " ELASTICA_DOXYGEN_HTML_EXTRA_STYLESHEETS
        "doxygen-awesome-css/doxygen-awesome.css"
        "doxygen-awesome-css/doxygen-awesome-sidebar-only.css"
        "doxygen-awesome-css/doxygen-awesome-sidebar-only-darkmode-toggle.css")

string(JOIN " \\\n    " ELASTICA_DOXYGEN_HTML_EXTRA_FILES
        "${ELASTICA_DOCS_THEME_DIR}/doxygen-awesome-darkmode-toggle.js"
        "${ELASTICA_DOCS_THEME_DIR}/doxygen-awesome-fragment-copy-button.js"
        "${ELASTICA_DOCS_THEME_DIR}/doxygen-awesome-interactive-toc.js")

file(MAKE_DIRECTORY "${ELASTICA_DOCS_THEME_DIR}")
file(COPY
        "${AWESOME_CSS_DIR}/doxygen-awesome.css"
        "${AWESOME_CSS_DIR}/doxygen-awesome-sidebar-only.css"
        "${AWESOME_CSS_DIR}/doxygen-awesome-sidebar-only-darkmode-toggle.css"
        "${AWESOME_CSS_DIR}/doxygen-awesome-darkmode-toggle.js"
        "${AWESOME_CSS_DIR}/doxygen-awesome-fragment-copy-button.js"
        "${AWESOME_CSS_DIR}/doxygen-awesome-interactive-toc.js"
        DESTINATION "${ELASTICA_DOCS_THEME_DIR}")

configure_file(
        "${PROJECT_SOURCE_DIR}/docs/Doxyfile.in"
        "${ELASTICA_DOCS_DOXYFILE}"
        @ONLY)

add_custom_target(
        doc
        COMMAND "${CMAKE_COMMAND}" -E echo "[doc] Starting Doxygen build."
        COMMAND "${CMAKE_COMMAND}" -E echo "[doc] Warnings will be written to: ${ELASTICA_DOCS_WARN_LOG}"
        COMMAND "${CMAKE_COMMAND}" -E echo "[doc] HTML output: ${ELASTICA_DOCS_OUTPUT_DIR}"
        COMMAND "${DOXYGEN_EXECUTABLE}" "${ELASTICA_DOCS_DOXYFILE}"
        WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/docs"
        DEPENDS "${ELASTICA_DOCS_DOXYFILE}"
        COMMENT "Generating API documentation with Doxygen"
        USES_TERMINAL
        VERBATIM)

message(STATUS "Documentation target enabled")
message(STATUS "Documentation output directory: ${ELASTICA_DOCS_OUTPUT_DIR}")
