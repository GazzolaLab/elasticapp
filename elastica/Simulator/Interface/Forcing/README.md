## Forcing

This folder forms the interface to the Forcing functionality in `Elastica++`. The functionality is split into multiple
files/logical entites, with different intentions:

- [Module](Module.hpp)
    - Responsible for enabling Forcing specific functionality to the Simulator
    - Present an interface that "coordinates" components of interface and algorithm
    - Store data for these components.
- [Configuration](Configuration.hpp)
    - Checks whether the Traits passed to [Module](Module.hpp) above make sense for the module, else error out
- [Forcing](Forcing.hpp)
    - Derived from [ForcingBase](ForcingBase.hpp) purely for runtime-polymorphism.
    - Is the actual interface for Forcing, and contains the option to add a specific type of Forcing at what indices and
      so on.
    - Clients work with these, but since its an implementation detail, we don't expose it to users
    - Once the Simulator is finalized(), this is thrown away.
- [Forcer](Forcer.hpp)
    - Derived from [ForcerBase](ForcerBase.hpp) purely for runtime-polymorphism.
    - Actually implements the functionality of a Forcing, by storing the system to which the Forcing is applied, the
      information about the indices etc.
    - Looping/Ranging over indices are taken care internally, so that users only work with one index at a time.
    - This is a persistent object within the Simulator and the [Module](Module.hpp) only works with this.
- [ExecutionPolicy](ExecutionPolicy.hpp)
    - Abstracts the module from the details of the algorithm to employ based on the parallelism requested, or strategy
      employed.
- [Serialize](Serialize.hpp)
    - Serialization of module, which then recursively serializes its contents.
