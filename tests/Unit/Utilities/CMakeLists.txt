# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY "Test_Utilities")

set(LIBRARY_SOURCES
        # Test_StdHelpers.cpp
        # Test_TaggedTuple.cpp
        # Test_Tuple.cpp
        Test_AsConst.cpp
        Test_AsTaggedTuple.cpp
        Test_Clamp.cpp
        Test_CRTP.cpp
        Test_Defer.cpp
        Test_End.cpp
        Test_FakeVirtual.cpp
        Test_GetOutput.cpp
        Test_Invoke.cpp
        Test_KarmarkarKarpPartition.cpp
        Test_MakeFromTuple.cpp
        Test_MakeNamedFunctor.cpp
        Test_NumericAt.cpp
        Test_NumericRange.cpp
        Test_Overloader.cpp
        Test_ProtocolHelpers.cpp
        # Test_PtrVector.cpp
        Test_PrettyType.cpp
        Test_Registration.cpp
        Test_RepeatAsTuple.cpp
        Test_Requires.cpp
        Test_Size.cpp
        Test_StaticConst.cpp
        Test_StdHelpers.cpp
        Test_StdVectorHelpers.cpp
        Test_SystemClock.cpp
        Test_TaggedTuple.cpp
        Test_TMPL.cpp
        Test_TypeTraits.cpp)

add_test_library(${LIBRARY}
        "Utilities"
        "${LIBRARY_SOURCES}"
        "Utilities;Options;ErrorHandling")

add_subdirectory(Math/BlazeDetail)
add_subdirectory(Geometry)
add_subdirectory(NamedType)
add_subdirectory(TypeTraits)
add_subdirectory(Math)
