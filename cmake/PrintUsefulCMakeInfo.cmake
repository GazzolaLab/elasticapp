# Prints information for ease in backtracking

# Obtained with thanks from https://github.com/sxs-collaboration/spectre

# Distributed under the MIT License. See LICENSE for details.

log_info("\nProject Information")
log_info("------------------")
log_info("-> PROJECT_NAME:           ${PROJECT_NAME}")
log_info("-> PROJECT_FULL_VERSION:   ${PROJECT_FULL_VERSION}")
log_info("-> PROJECT_VERSION:        ${PROJECT_VERSION}")
log_info("-> PROJECT_VERSION_MAJOR:  ${PROJECT_VERSION_MAJOR}")
log_info("-> PROJECT_VERSION_MINOR:  ${PROJECT_VERSION_MINOR}")
log_info("-> PROJECT_VERSION_PATCH:  ${PROJECT_VERSION_PATCH}")
log_info("-> GIT_BRANCH:             ${GIT_BRANCH}")
log_info("-> GIT_COMMIT_HASH:        ${GIT_COMMIT_HASH}")
log_info("-> GIT_TAG:                ${GIT_TAG}")

log_info("\nBuild Information")
log_info("------------------")
log_info("-> BUILD_DATE:            ${BUILD_DATE}")
log_info("-> LIBRARY_NAME:          ${LIBRARY_NAME}")
log_info("-> BUILD_STATIC_LIBRARY      ${BUILD_STATIC_LIBRARY}")
log_info("-> BUILD_SHARED_LIBRARY      ${BUILD_SHARED_LIBRARY}")
log_info("-> BUILD_EXAMPLES            ${BUILD_EXAMPLES}")
log_info("-> BUILD_BENCHMARKS          ${BUILD_BENCHMARKS}")
log_info("-> BUILD_TESTS               ${BUILD_TESTS}")
log_info("-> Build Directory:  ${CMAKE_BINARY_DIR}")
log_info("-> Source Directory:  ${CMAKE_SOURCE_DIR}")
log_info("-> Bin Directory:  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
log_info("-> CMake Modules Path:  ${CMAKE_MODULE_PATH}")
log_info("-> Operating System:  ${CMAKE_SYSTEM_NAME}")

log_info("\nCompiler Information")
log_info("------------------")
log_info("-> CMAKE_CXX_COMPILER              ${CMAKE_CXX_COMPILER}")
log_info("-> CMAKE_CXX_COMPILER_ID           ${CMAKE_CXX_COMPILER_ID}")
log_info("-> CMAKE_CXX_COMPILER_VERSION      ${CMAKE_CXX_COMPILER_VERSION}")
log_info("-> CMAKE_CXX_FLAGS:  ${CMAKE_CXX_FLAGS}")
log_info("-> CMAKE_CXX_LINK_FLAGS:  ${CMAKE_CXX_LINK_FLAGS}")
log_info("-> CMAKE_CXX_FLAGS_DEBUG:  ${CMAKE_CXX_FLAGS_DEBUG}")
log_info("-> CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
log_info("-> CMAKE_EXE_LINKER_FLAGS:  ${CMAKE_EXE_LINKER_FLAGS}")
log_info("-> CMAKE_BUILD_TYPE:  ${CMAKE_BUILD_TYPE}")
log_info("-> ELASTICA_LIBRARIES: ${ELASTICA_LIBRARIES}")
log_info("-> USE_SYSTEM_INCLUDE: ${USE_SYSTEM_INCLUDE}")
# message(STATUS "USE_PCH: ${USE_PCH}")

# if (PYTHONINTERP_FOUND) message(STATUS "Python: " ${PYTHON_EXECUTABLE})
# message(STATUS "Python Version: ${PYTHON_VERSION_STRING}") else()
# message(STATUS "Python: Not found") endif()

# if(CLANG_TIDY_BIN) message(STATUS "Found clang-tidy: ${CLANG_TIDY_BIN}")
# elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang") message( STATUS "Could not find
# clang-tidy even though LLVM clang is installed" ) endif()

# if (CODE_COVERAGE) message(STATUS "Code coverage enabled. All prerequisites
# found:") message(STATUS "  gcov: ${GCOV}") message(STATUS "  lcov: ${LCOV}")
# message(STATUS "  genhtml: ${GENHTML}") message(STATUS "  sed: ${SED}")
# endif()

# if(DOXYGEN_FOUND) message(STATUS "Doxygen: " ${DOXYGEN_EXECUTABLE}) else()
# message(STATUS "Doxygen: Not found, documentation cannot be built.") endif()
