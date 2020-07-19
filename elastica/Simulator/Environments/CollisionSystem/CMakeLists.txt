# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CollisionSystem)

add_library(${LIBRARY})

target_sources(
        ${LIBRARY}
        PRIVATE
        ContactType.cpp
        Contact/Contact.cpp
        Contact/ContactBase.cpp
        Response/DEMForces/LinearSpringDashpot.cpp
)

target_include_directories(
        ${LIBRARY}
        PUBLIC
        Batches
        Configuration
        Contact
        Detection
        Response
        .
)

target_link_libraries(
        ${LIBRARY}
        PUBLIC
        Primitives
        ErrorHandling
        Utilities
)
