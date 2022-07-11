 set(LIBRARY Time)

 elastica_target_headers(
         ${LIBRARY}
         INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
         HEADERS
         Types.hpp
         TypeTraits.hpp
         Update.hpp
 )
