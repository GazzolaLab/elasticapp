## Systems

This folder forms the interface to adding/querying Systems in `Elastica++`. The functionality is split into multiple
files/logical entites, with different intentions:

- [Module](Module.hpp)
    - Responsible for enabling (compulsary) Systems specific functionality to the Simulator
    - Present an interface to add and query information about Systems registered in the simulator.
    - Stores systems data.
- Configuration aka (Configuration.hpp)
    - Is notably absent because the SimulatorConfiguration necessarily needs to pre-process and check for the existence
      of a Systems configuration. So we can safely skip the checks here.
- [ExecutionPolicy](ExecutionPolicy.hpp)
    - Abstracts and decides the algorithm to employ for iterating over the types of system (such as a `CosseratRod`
      , `CosseratRodWithDifferentElasticityModel`, `Sphere`) based on the parallelism requested.
    - In addition, it also decides how to iterate within a collection of a type of system (such as looping over
      the `Sphere`s inside a vector)
- [Serialize](Serialize.hpp)
    - Serialization of systems, which then recursively serializes its contents.
