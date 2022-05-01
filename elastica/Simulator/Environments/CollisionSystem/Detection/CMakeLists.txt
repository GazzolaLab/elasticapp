set(LIBRARY CollisionDetection)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(Coarse)
add_library(${LIBRARY}::CoarseCollisionDetection ALIAS CoarseCollisionDetection)

add_subdirectory(ContactCache)
add_library(${LIBRARY}::ContactCache ALIAS ContactCache)

add_subdirectory(Fine)
add_library(${LIBRARY}::FineCollisionDetection ALIAS FineCollisionDetection)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Coarse.hpp
        Fine.hpp
        PairwiseTMPL.hpp
        Detection.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Coarse.hpp
        Fine.hpp
        PairwiseTMPL.hpp
        Detection.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        CoarseCollisionDetection
        FineCollisionDetection
        ContactCache
        Utilities # Tmpl
)
