## Connections

This folder implements functionality to make new Connections in `Elastica++` and uses these to define a canned set of
Connections for quick re-use. The functionality is split into multiple files/logical entities. The main ones are:

- [Connection](Connection.hpp)
    - Contains the main Connection file that establishes an interface from a Connection to the rest of the simulator.
    - Includes code generation based on the user defined connection type.
    - Clients simply need to derive from this class using the CRTP pattern and implement a couple of functions.
- [MakeConnection](MakeConnection.hpp)
    - Implementing own Connections with all the boiler plate is tedious---It'll be nice to automate this process and
      implement Connections from a couple of lambda functions.
    - This is possible using MakeConnection---internally, this builds on a Connection and takes in two lambda functions
      with a specific signature.
- [Protocols](Protocols.hpp)
    - Protocols for a Connection in case advanced users needs more flexibility in customization.
    - Our basic [Connection](Connection.hpp) already conforms to this protocol, so casual users do not need to do
      anything more other than the CRTP
- [Registration](Registration.hpp)
    - In case runtime support for Connections is required (such as while scripting new Connections in Python or loading
      data from a YAML configuration file into a canned Connection), one needs to register the Connections using the
      Registrar/Registration mechanism.
    - This file has helpful macros to ease registration of such new Connections. Its very useful if you do not want to
      bother yourself with boring registration details. All canned Connection classes (written in the style of deriving
      from [Connection](Connection.hpp), needed for runtime data support) use these helpers.
- [Canned](Canned.hpp)
    - This file and the associated folder contains all canned Connection algorithms within `Elastica++`.
