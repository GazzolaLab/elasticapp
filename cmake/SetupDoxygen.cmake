# Distributed under the MIT License.
# See LICENSE.txt for details.

log_info("Finding Doxygen")

find_package(Doxygen REQUIRED)

if (DOXYGEN_FOUND)
    log_debug("DOXYGEN_VERSION: ${DOXYGEN_VERSION}")

    set(ELASTICA_DOX_GENERATE_HTML "YES")
    set(ELASTICA_DOX_GENERATE_XML "NO")

    configure_file(
            docs/Doxyfile.in
            ${PROJECT_BINARY_DIR}/docs/DoxyfileHtml @ONLY IMMEDIATE
    )

    # Construct the command that calls Doxygen
    set(
            GENERATE_DOCS_COMMAND
            "${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/docs/DoxyfileHtml"
    )

    # Parse the command into a CMake list for the `add_custom_target`
    separate_arguments(GENERATE_DOCS_COMMAND)

    add_custom_target(
            doc
            COMMAND ${GENERATE_DOCS_COMMAND}
            DEPENDS
            ${PROJECT_BINARY_DIR}/docs/DoxyfileHtml
            COMMENT
            "Generating API documentation with Doxygen"
            VERBATIM
    )

else (DOXYGEN_FOUND)
    message(WARNING "Doxygen is needed to build the documentation.")
endif (DOXYGEN_FOUND)
