# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_CollisionSystem_Contact")

set(LIBRARY_SOURCES
        Test_Contact.cpp
        Test_ContactType.cpp
        Test_ContactVector.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/CollisionSystem/Contact"
        "${LIBRARY_SOURCES}"
        "Contact;Primitives;Utilities"
)
