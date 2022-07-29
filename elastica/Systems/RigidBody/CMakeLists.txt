 set(LIBRARY RigidBody)

 add_elastica_library(${LIBRARY} INTERFACE)

 set(LIBRARY_SOURCES
         Access.hpp
         Aliases.hpp
         Block.hpp
         BlockInitializer.hpp
         BlockSlice.hpp
         BlockView.hpp
         Components.hpp
         RigidBody.hpp
         RigidBodyPlugin.hpp
         RigidBodyTraits.hpp
         Sphere.hpp
         Tags.hpp
         Types.hpp
         )

 elastica_target_headers(
         ${LIBRARY}
         INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
         ${LIBRARY_SOURCES}
 )

 elastica_target_sources(
         ${LIBRARY}
         INTERFACE
         ${LIBRARY_SOURCES}
 )

 add_subdirectory(Aliases)
 add_library(${LIBRARY}::Aliases ALIAS RigidBodyAliases)

 add_subdirectory(Components)
 add_library(${LIBRARY}::Components ALIAS RigidBodyComponents)

 add_subdirectory(Sphere)
 add_library(${LIBRARY}::Sphere ALIAS RigidBodySphereComponents)

 add_subdirectory(Traits)
 add_library(${LIBRARY}::Traits ALIAS RigidBodyTraits)

 # FIXME refactor serialization
 target_link_libraries(
         ${LIBRARY}
         INTERFACE
         RigidBodyAliases
         RigidBodyComponents
         RigidBodySphereComponents
         RigidBodyTraits
         Serialization
 )
