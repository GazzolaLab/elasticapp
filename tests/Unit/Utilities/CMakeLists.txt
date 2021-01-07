# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY "Test_Utilities")

set(LIBRARY_SOURCES
        # Test_StdHelpers.cpp
        # Test_TaggedTuple.cpp
        # Test_Tuple.cpp
        Test_CRTP.cpp
        Test_FakeVirtual.cpp
        Test_Invoke.cpp
        Test_Overloader.cpp
        Test_ProtocolHelpers.cpp
        Test_PtrVector.cpp
        Test_PrettyType.cpp
        Test_Registration.cpp
        Test_Requires.cpp
        Test_TMPL.cpp
        Test_TypeTraits.cpp)

add_test_library(${LIBRARY}
        "Utilities"
        "${LIBRARY_SOURCES}"
        "Utilities;Options;ErrorHandling")

add_subdirectory(NamedType)
add_subdirectory(TypeTraits)