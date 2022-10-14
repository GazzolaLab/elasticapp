set(LIBRARY "Test_CosseratRod_Traits_Operations_BlazeBackend")

set(LIBRARY_SOURCES
        Test_CalculusKernels.cpp
        Test_LinearAlgebraKernels.cpp
        Test_OpsTraits.cpp
        Test_InvRotateDivide.cpp
        Test_InvRotateDivide_Scalar.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods/Traits/Operations/BlazeBackend"
        "${LIBRARY_SOURCES}"
        "CosseratRodBlazeOperations;ModuleSettings"
)
