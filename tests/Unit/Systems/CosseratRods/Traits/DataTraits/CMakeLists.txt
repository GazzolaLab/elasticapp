add_subdirectory(BlazeBackend)

set(LIBRARY "Test_CosseratRod_Traits_DataTraits")

set(LIBRARY_SOURCES
        Test_Protocols.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods/Traits/DataTraits"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Systems"
)
