## Parallel

All parallelism abstractions go in this folder. These abstractions present a minimal, sensible parallel interface to
program `Elastica++`, indepedent of the implementation details (such as SMP using TBB/OpenMP or MPI parallelism). These
abstractions are then used inside the simulator to generate parallel code.