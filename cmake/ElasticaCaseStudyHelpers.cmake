cmake_minimum_required(VERSION 3.13)

# Add this file lazily to prevent unwanted dependencies
if (NOT TARGET elastica_standalone_executable)
    # some other file!
    add_library(elastica_standalone_executable INTERFACE)
    # setup cxx opts
    include(SetupCxxOpts)
    target_link_libraries(elastica_standalone_executable INTERFACE cxxopts::cxxopts)
    target_link_libraries(elastica_standalone_executable INTERFACE _elastica_standalone_support)
    add_library(elastica::standalone_executable ALIAS elastica_standalone_executable)
endif ()

function(_elastica_setup_standalone_executable TARGET SRCLIB CASE_STUDY CASE_STUDY_APPLICATION_NAME INCLUDES)
    string(CONFIGURE [[
        /*
        *
        * Warning :
        * This is an Elastica++ generated file!
        * Edit at your own peril.
        */
        @INCLUDES@
        #include <elastica/Utilities/AppSupport/CaseStudyParser.hpp>

        int main(int argc, char* argv[]) {
            using CaseStudy = @CASE_STUDY@;
            char const* case_study_name("@CASE_STUDY_APPLICATION_NAME@");

            auto input_parameters =
            ::standalone_bindings::parse_case_study_parameters<CaseStudy>(case_study_name, argc, argv);

            CaseStudy case_study{};
            case_study.setup(input_parameters);
            case_study.run();

            return EXIT_SUCCESS;
        }
    ]] code @ONLY)

    set(GENERATED_SRC_FILE_NAME "standalone_${TARGET}")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${GENERATED_SRC_FILE_NAME}.cpp" "${code}")

    add_executable(${TARGET} "${CMAKE_CURRENT_BINARY_DIR}/${GENERATED_SRC_FILE_NAME}.cpp")
    target_link_libraries(${TARGET} PRIVATE ${SRCLIB})
    # Takes care of including everything thats not to do with the target
    target_link_libraries(${TARGET} PUBLIC elastica::standalone_executable)
endfunction()

# function(_elastica_setup_python_executable TARGET SRCLIB CASE_STUDY CASE_STUDY_APPLICATION_NAME INCLUDES)
# endfunction()

function(_elastica_setup_case_study TYPE)
    if (${TYPE} STREQUAL "elastica::standalone_executable")
        _elastica_setup_standalone_executable(${ARGN})
    # elseif (${TYPE} STREQUAL "elastica::python_object")
    #     _elastica_setup_python_executable(${ARGN})
    else ()
        message(FATAL_ERROR "unrecognized TYPE in a Elastica case study")
    endif ()
endfunction()

function(_elastica_register_case_names_from SOURCE_FILE CASE_NAMES)
    if (NOT EXISTS ${SOURCE_FILE})
        message(WARNING "Could not find source file:\n\"${SOURCE_FILE}\"\nfor \
        case studies\n.")
        return()
    endif ()

    file(STRINGS
            ${SOURCE_FILE}
            CONTENTS
            NEWLINE_CONSUME)


    # Remove commented out case studies so they are not added
    string(REGEX
            REPLACE "\n[ \t]*//+[^\n]+"
            "\n"
            CONTENTS
            "${CONTENTS}")

    #    message(${CONTENTS})

    # The regex searches for ELASTICA_CASE_STUDY to find  case studies that need to be added.
    # TESTS will be a list of all case studies found in the SOURCE_FILE.
    string(
            REGEX
            MATCHALL
            "ELASTICA_CASE_STUDY[ \t]*\\([^\)]+\\)[ \t]*"
            CASES
            "${CONTENTS}")

    foreach (CASE_NAME ${CASES})
        # Finds the case name and returns (Casename)
        string(REGEX
                MATCH "\\((.*)\\)"
                CASE_NAME_STRIPPED
                "${CASE_NAME}"
                )
        # Strips away the ()
        string(REGEX
                REPLACE "\\((.*)\\)"
                "\\1"
                CASE_NAME_STRIPPED
                "${CASE_NAME_STRIPPED}")
        list(APPEND STRIPPED_CASE_NAMES ${CASE_NAME_STRIPPED})
    endforeach ()

    set(CASE_NAMES ${STRIPPED_CASE_NAMES} PARENT_SCOPE)

endfunction()

# Adds a target TARGET as a case study.
#
# Usage
#   add_elastica_case_study(
#     TARGET TARGET_NAME
#     FROM GENERATOR_NAME
#     TYPE ElasticaExecutableType
#     )
#
# Description
# Given a case-study generator, it adds a case-study as a target and compiles
# it to a standalone commandline executable or a Python library.
#
# Example
# # These are client's generator files
# set(GENERATOR "my_generator")
# add_library(${GENERATOR} STATIC my_generator.cpp)
# target_sources(${GENERATOR} INTERFACE my_generator.hpp)
#
# # To add a "standalone" executable called "my_cmdline_exe"
# # generator from "my_generator"
# add_elastica_case_study(my_cmdline_exe
#         FROM my_generator
#         AS elastica::standalone_executable)
#
# # To add an executable accessible in "python" called "my_python_exe"
# # generator from "my_generator"
# add_elastica_case_study(my_python_exe
#         FROM my_generator
#         AS elastica::python_object)
function(add_elastica_case_study TARGET)
    # Usage
    ##
    # Parse the arguments and set defaults for missing values.
    ##
    set(options NAME)
    set(oneValueArgs FROM AS)
    set(multiValueArgs)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT "${ARG_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(AUTHOR_WARNING "Arguments to add_elastica_case_study not recognized: ${ARG_UNPARSED_ARGUMENTS}")
    endif ()

    if (NOT ARG_FROM)
        message(FATAL_ERROR "Missing FROM argument specifying a Elastica case study generator target")
    endif ()

    if (NOT ARG_AS)
        message(FATAL_ERROR "Missing AS argument specifying a Elastica case study generator target")
    endif ()

    if (NOT ARG_NAME)
        set(ARG_NAME "${TARGET}")
    endif ()

    get_target_property(INTERFACE_FILES ${ARG_FROM} INTERFACE_SOURCES)

    # Register the name of the case study class
    foreach (INTERFACE_FILE IN LISTS INTERFACE_FILES)
        _elastica_register_case_names_from(${INTERFACE_FILE} CASE_NAMES)
    endforeach ()

    list(LENGTH CASE_NAMES N_CASE_NAMES)

    if (${N_CASE_NAMES} LESS 1)
        message(FATAL_ERROR "No case study found in the headers, as registered with the ELASTICA_CASE_STUDY() macro")
    endif ()

    if (${N_CASE_NAMES} GREATER 1)
        message(FATAL_ERROR "A case study header can have only one class registered with the ELASTICA_CASE_STUDY() macro! Please refactor your code...")
    endif ()

    list(GET CASE_NAMES 0 REGISTERED_CASE_NAME)

    # Add the interfaces as header dependencies
    list(TRANSFORM INTERFACE_FILES PREPEND "#include \"" OUTPUT_VARIABLE INTERFACE_INCLUDES)
    list(TRANSFORM INTERFACE_INCLUDES APPEND "\"")
    foreach (INTERFACE_INCLUDE IN LISTS INTERFACE_INCLUDES)
        string(APPEND INTERFACE_INCLUDES_STR "${INTERFACE_INCLUDE}\n")
    endforeach ()


    _elastica_setup_case_study(${ARG_AS} ${TARGET} ${ARG_FROM} ${REGISTERED_CASE_NAME} ${ARG_NAME} ${INTERFACE_INCLUDES_STR})
endfunction()