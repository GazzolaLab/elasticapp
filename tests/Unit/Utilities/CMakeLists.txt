# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY "Test_Utilities")

set(LIBRARY_SOURCES
    # Test_Requires.cpp
    # Test_StdHelpers.cpp
    # Test_TaggedTuple.cpp
    # Test_TMPL.cpp
    # Test_Tuple.cpp
    Test_TypeTraits.cpp)

add_test_library(${LIBRARY}
                 "Utilities"
                 "${LIBRARY_SOURCES}"
                 "Utilities")
