set(BENCHMARK "BenchmarkInvRotateDivide")
set(PROFILE "ProfileInvRotateDivide")

add_executable(${BENCHMARK} BenchmarkInvRotateDivide.cpp)
add_executable(${PROFILE} ProfileInvRotateDivide.cpp)

foreach (EXEC ${BENCHMARK};${PROFILE})
    target_link_libraries(${EXEC}
            PUBLIC
            Utilities
            ErrorHandling
            Blaze
            BlazeTensor
            Systems
            ElasticaFlags
            # Sleef
            )

endforeach ()
