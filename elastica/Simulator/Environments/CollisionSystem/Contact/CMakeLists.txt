set(LIBRARY Contact)

add_elastica_library(${LIBRARY})

add_subdirectory(ContainerPolicy)
add_library(${LIBRARY}::ContainerPolicy ALIAS ContactContainerPolicy)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Contact.hpp
        ContactBase.hpp
        ContactTrait.hpp
        ContactType.hpp
        ContactVectorAdapter.hpp
        ContactContainerTrait.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        Contact.cpp
        ContactBase.hpp
        ContactBase.cpp
        ContactType.cpp
        PUBLIC
        Contact.hpp
        ContactTrait.hpp
        ContactType.hpp
        ContactVectorAdapter.hpp
        ContactContainerTrait.hpp
)

target_link_libraries(
        ${LIBRARY}
        PUBLIC
        Utilities
        Materials
        PRIVATE
        Primitives # needs only the interface I think
        ErrorHandling
)