# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Transfer")

set(LIBRARY_SOURCES
        Test_Transfer.cpp
        Test_TypeTraits.cpp
        Test_Transferer.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/Transfer"
        "${LIBRARY_SOURCES}"
        "Transfer;Parallel" # FIXME : Parallel is not transitive
)
