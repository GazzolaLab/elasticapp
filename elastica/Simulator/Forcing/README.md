## Forcing

This folder implements functionality to make new Forcing in `Elastica++` and uses these to define a canned set of
Forcing for quick re-use. The functionality is split into multiple files/logical entities. The main ones are:

- [Forcing](Forcing.hpp)
    - Contains the main Forcing file that establishes an interface from a Forcing to the rest of the simulator.
    - Includes code generation based on the user defined connection type.
    - Clients simply need to derive from this class using the CRTP pattern and implement a couple of functions.
- [MakeForcing](MakeForcing.hpp)
    - Implementing own Forcing with all the boiler plate is tedious---It'll be nice to automate this process and
      implement Forcing from a couple of lambda functions.
    - This is possible using MakeForcing---internally, this builds on a Forcing and takes in two lambda functions
      with a specific signature.
- [Protocols](Protocols.hpp)
    - Protocols for a Forcing in case advanced users needs more flexibility in customization.
    - Our basic [Forcing](Forcing.hpp) already conforms to this protocol, so casual users do not need to do
      anything more other than the CRTP
- [Registration](Registration.hpp)
    - In case runtime support for Forcing is required (such as while scripting new Forcing in Python or loading
      data from a YAML configuration file into a canned Forcing), one needs to register the Forcing using the
      Registrar/Registration mechanism.
    - This file has helpful macros to ease registration of such new Forcing. Its very useful if you do not want to
      bother yourself with boring registration details. All canned Forcing classes (written in the style of deriving
      from [Forcing](Forcing.hpp), needed for runtime data support) use these helpers.
- [Canned](Canned.hpp)
    - This file and the associated folder contains all canned Forcing algorithms within `Elastica++`.
