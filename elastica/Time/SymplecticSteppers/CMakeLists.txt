 set(LIBRARY Time)

 elastica_target_headers(
         ${LIBRARY}
         INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
         HEADERS
         Algorithms.hpp
         SymplecticStepper.hpp
         StepperTags.hpp
         Types.hpp
 )

 add_subdirectory(Algorithms)
