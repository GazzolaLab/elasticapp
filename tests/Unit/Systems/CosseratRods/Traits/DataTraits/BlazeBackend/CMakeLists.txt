set(LIBRARY "Test_CosseratRod_Traits_DataTraits_BlazeBackend")

set(LIBRARY_SOURCES
        Test_MatrixTag.cpp
        Test_TensorTag.cpp
        Test_VectorTag.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods/Traits/DataTraits/BlazeBackend"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Systems"
)
