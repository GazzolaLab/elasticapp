## Constraints

This folder implements functionality to make new Constraints in `Elastica++` and uses these to define a canned set of
Constraints for quick re-use. The functionality is split into multiple files/logical entities. The main ones are:

- [Constraint](Constraint.hpp)
    - Contains the main Constraint file that establishes an interface from a Constraint to the rest of the simulator.
    - Includes code generation based on the user defined connection type.
    - Clients simply need to derive from this class using the CRTP pattern and implement a couple of functions.
- [MakeConstraint](MakeConstraint.hpp)
    - Implementing own Constraints with all the boiler plate is tedious---It'll be nice to automate this process and
      implement Constraints from a couple of lambda functions.
    - This is possible using MakeConstraint---internally, this builds on a Constraint and takes in two lambda functions
      with a specific signature.
- [Protocols](Protocols.hpp)
    - Protocols for a Constraint in case advanced users needs more flexibility in customization.
    - Our basic [Constraint](Constraint.hpp) already conforms to this protocol, so casual users do not need to do
      anything more other than the CRTP
- [Registration](Registration.hpp)
    - In case runtime support for Constraints is required (such as while scripting new Constraints in Python or loading
      data from a YAML configuration file into a canned Constraint), one needs to register the Constraints using the
      Registrar/Registration mechanism.
    - This file has helpful macros to ease registration of such new Constraints. Its very useful if you do not want to
      bother yourself with boring registration details. All canned Constraint classes (written in the style of deriving
      from [Constraint](Constraint.hpp), needed for runtime data support) use these helpers.
- [Canned](Canned.hpp)
    - This file and the associated folder contains all canned Constraint algorithms within `Elastica++`.
