## Timesteppers

This folder forms the interface to the Timestepping functionality of an `Elastica++` simulator. Since the simulator is a
composition of multiple systems, we pass in a time-stepper as a function object and walk/step it through the simulator
using a `step()` method. Note that this can be done inside a free `integrate()` function too---but this form seems more
natural with the `for_each...()` exposed by the `Systems` module.

The functionality is split into multiple files/logical entites, with different intentions:

- [SymplecticSteppers](SymplecticSteppers.hpp)
    - Provides a `step()` method for use with any symplectic time-stepping algorithm. It assumes implicitly that each
      system exposes the interface needed by a SymplecticStepper and does not check for it. This assumption is
      beneficial since the `step()` method only instantiates with an appropriate time-stepper, hence if `step()` is not
      used (for example, if systems are integrated with an ExplicitStepper), then there's no need for the systems to
      satisfy the assumption, and the client does not need to conform to the interface.
- [ExplicitSteppers](ExplicitSteppers.hpp)
    - Provides a `step()` method for use with any symplectic time-stepping algorithm. It assumes implicitly that each
      system exposes the interface needed by a ExplicitStepper and does not check for it. This assumption is
      beneficial since the `step()` method only instantiates with an appropriate time-stepper, hence if `step()` is not
      used (for example, if systems are integrated with an SymplecticStepper), then there's no need for the systems to
      satisfy the assumption, and the client does not need to conform to the interface.
