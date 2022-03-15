#!/usr/bin/env python3

# Distributed under the MIT License.
# See LICENSE.txt for details.

import re

# A list of all the allowed ctest labels/Catch tags for tests
allowed_tags = [
    "Connections",
    "Constraints",
    "Configuration",
    "Domain",
    "ErrorHandling",
    "Forcing",
    "IO",
    "Informer",
    "Integrated",
    "Interface",
    "Options",
    "Parallel",
    "Serialization",
    "Simulator",
    "Systems",
    "Time",
    "Unit",
    "Utilities",
]

# Allowed test attributes
allowed_test_attributes = ["TimeOut",
                           "OutputRegex"]

# All the timeout times for the different types of tests. The order here
# matters. Whichever time is specified last is what will be used for the
# test if it is of multiple types.
default_tag_timeouts = [("unit", 5), ("integrated", 15)]

allowed_tags = [x.lower() for x in allowed_tags]
allowed_test_attributes = [x.lower() for x in allowed_test_attributes]


def parse_source_file(file_name):
    # Read the file and remove include directives to make life easier.
    file_string = str(
        re.compile("#include.*\n").sub("", str(open(file_name, "r").read())))

    # The (.*?); part of the regex is to capture the first line of the test
    # body. However, if the test case does not (yet) contain any semicolons then
    # we accidentally fail to find a test.
    test_regex = re.compile(
        "(\/\/ \[\[.*?)?ELASTICA_TEST_CASE\((.*?)\) {(.*?);", re.DOTALL)
    test_cases_found = re.findall(test_regex, file_string)

    if not test_cases_found and not "static_assert" in file_string:
        print("\n\nERROR!!!\nFailed to find any test cases in the file ",
              file_name)
        print("This occurs when neither a static_assert nor any "
              "ELASTICA_TEST_CASE are found. You may incorrectly hit this error"
              " message if your test case does not yet contain any code. "
              "Specifically, if your test case does not contain a semicolon."
              "\n\n\n\n")
        sys.exit(1)

    for (attributes, test_name, test_body_first_line) in test_cases_found:
        # Capture the name of the test into the first group and the tags into
        # the second, the first sentence into third group
        # For example,
        # "Unit.My.Test", "[Unit][My]"){ "hai"
        # group(1) == Unit.My.Test
        # group(2) == [Unit][My]
        # group(3) == hai
        parsed_name = re.search("\"(.*)\",[\s]*\"(.*)\"", test_name)
        try:
            test_name = parsed_name.group(1)
            test_tags = parsed_name.group(2).lower().replace("[", "")[:-1]
            test_tags = test_tags.split("]")
        except AttributeError as e:
            # in case there is a commented out or mismatched regex
            print("Error raised for test named %s in file " % (test_name, file_name))
            raise e
        for test_tag in test_tags:
            if not test_tag in allowed_tags:
                print("\nERROR: The tag '%s' is not allowed but was found in "
                      "the test '%s' in the file '%s'. To allow it add it to "
                      "the 'allowed_tags' list in "
                      "$ELASTICA_ROOT/cmake/ElasticaParseTests.py. The currently "
                      "allowed tags are:\n%s\n\n" %
                      (test_tag, test_name, file_name, allowed_tags))
                exit(1)
        test_timeout = -1
        for (tag, timeout) in default_tag_timeouts:
            if tag in test_tags:
                test_timeout = timeout

        # Parse the test attributes
        should_have_output_regex = "ERROR_TEST()" in test_body_first_line \
                                   or "ASSERTION_TEST()" in test_body_first_line \
                                   or "OUTPUT_TEST()" in test_body_first_line
        output_regex = ''

        all_attributes_by_name = re.findall("\[\[([^,]+), (.*?)\]\]",
                                            attributes, re.DOTALL)
        for attribute in all_attributes_by_name:
            if not attribute[0].lower() in allowed_test_attributes:
                print("\nERROR: Found unknown test attribute '%s' applied "
                      "to test '%s' in file %s." %
                      (attribute[0], test_name, file_name))
                exit(1)

            if attribute[0].lower() == "timeout":
                test_timeout = attribute[1]

            if attribute[0].lower() == "outputregex":
                if not should_have_output_regex:
                    print("\nERROR: The test '%s' in the file '%s' has the "
                          "attribute OutputRegEx, but does not contain the "
                          "macro ERROR_TEST(), ASSERTION_TEST(), or "
                          "OUTPUT_TEST() as its first line.\n" %
                          (test_name, file_name))
                    exit(1)
                output_regex = attribute[1].replace("\n//", "")

        if should_have_output_regex and output_regex == '':
            print("\nERROR: The test '%s' in the file '%s' was marked as an "
                  "ERROR_TEST(), ASSERTION_TEST(), or OUTPUT_TEST(), but "
                  "failed to produce a parsable OutputRegex attribute! "
                  "The syntax is // [[OutputRegex, <regular_expression>]] as "
                  "a comment before the ELASTICA_TEST_CASE.\n" %
                  (test_name, file_name))
            exit(1)

        open("%s.timeout" % test_name, "w").write("%s" % test_timeout)
        open("%s.output_regex" % test_name, "w").write("%s" % output_regex)


if __name__ == '__main__':
    import sys

    source_files = sys.argv[1:]
    for file in source_files:
        parse_source_file(file)
