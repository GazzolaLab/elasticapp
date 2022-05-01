# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Transfer)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Transfer.hpp
        Transferer.hpp
        TransferTags.hpp
        TransferTraits.hpp
        TransferWizard.hpp
        Types.hpp
        TypeTraits.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Transfer.hpp
        Transferer.hpp
        TransferTags.hpp
        TransferTraits.hpp
        TransferWizard.hpp
        Types.hpp
        TypeTraits.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
        ErrorHandling
)
