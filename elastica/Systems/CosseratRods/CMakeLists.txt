# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CosseratRods)
add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        Access.hpp
        Aliases.hpp
        Block.hpp
        BlockInitializer.hpp
        BlockSlice.hpp
        Components.hpp
        CosseratRodPlugin.hpp
        CosseratRods.hpp
        CosseratRodTraits.hpp
        Initializers.hpp
        Protocols.hpp
        Serialize.hpp # TODO : refactor
        Specialize.hpp
        Tags.hpp
        Types.hpp
        TypeTraits.hpp
        Utility.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Access.hpp
        Aliases.hpp
        Block.hpp
        BlockInitializer.hpp
        BlockSlice.hpp
        Components.hpp
        CosseratRodPlugin.hpp
        CosseratRods.hpp
        CosseratRodTraits.hpp
        Initializers.hpp
        Protocols.hpp
        Serialize.hpp # TODO : refactor
        Specialize.hpp
        Tags.hpp
        Types.hpp
        TypeTraits.hpp
        Utility.hpp
)

add_subdirectory(Access)
add_library(${LIBRARY}::Access ALIAS CosseratRodAccess)

add_subdirectory(Aliases)
add_library(${LIBRARY}::Aliases ALIAS CosseratRodAliases)

add_subdirectory(Components)
add_library(${LIBRARY}::Components ALIAS CosseratRodComponents)

add_subdirectory(Initializers)
add_library(${LIBRARY}::Initializers ALIAS CosseratRodInitializers)

add_subdirectory(Traits)
add_library(${LIBRARY}::Traits ALIAS CosseratRodTraits)

add_subdirectory(Utility)
add_library(${LIBRARY}::Utility ALIAS CosseratRodUtility)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        CosseratRodAccess
        CosseratRodAliases
        CosseratRodComponents
        CosseratRodInitializers
        CosseratRodTraits
        CosseratRodUtility
        Utilities
)

add_subdirectory(Python)