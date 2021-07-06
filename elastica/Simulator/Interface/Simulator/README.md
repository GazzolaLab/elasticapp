## Simulator

This folder forms the entry point into an `Elastica++` simulation. The simulator binds all the modules together with
common functionalities. 

- [Configuration](Configuration.hpp)
  - Checks whether the Traits passed to Simulator makes sense to setup the common functionality, else error out.
- [Simulator](Simulator.hpp)
    - Definition of an `Elastica++` simulator.
- [Serialize](Serialize.hpp)
    - Serialization of common utilities, and calls for serialize of its parent modules.
