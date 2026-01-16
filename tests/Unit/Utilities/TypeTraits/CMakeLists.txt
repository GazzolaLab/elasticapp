# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Utilities_TypeTraits")

set(LIBRARY_SOURCES
        Test_ArraySize.cpp
        Test_IsCRTP.cpp
        Test_InvokeResult.cpp
        Test_IsCallable.cpp
        Test_IsConvertibleToA.cpp
        Test_IsDetected.cpp
        Test_IsInvokable.cpp
        Test_IsStdArray.cpp
        )

add_test_library(
        ${LIBRARY}
        "Utilities/TypeTraits"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
