## Configuration

This folder has functionality to configure an `Elastica++` simulation. The functionality is split into multiple
files/logical entities. The main ones are:

- [ModuleConfiguration](ModuleConfiguration.hpp)
    - Catch all header that exposes functionality to customize each module
    - The basic premise here is that we construct the configuration for each module separately using this functionality
      and then funnel it into a common point aka [SimulatorConfiguration](SimulatorConfiguration.hpp)
- [SimulatorConfiguration](SimulatorConfiguration.hpp)
    - Takes the module specific configuration above and coordinates it in a sensible fashion.
    - Does some processing to figure out things that are common to all modules (such as Time)
    - This processing also involves manipulating types given in ModuleConfiguration
- [Defaults](Defaults.hpp)
    - For each module configuration listed in [ModuleConfiguration](ModuleConfiguration.hpp), this sets up a default
      configuration.
    - These will be used in the Simulator as a default argument.
- [DefaultSimulatorConfiguration](DefaultSimulatorConfiguration.hpp)
    - This is the [SimulatorConfiguration](SimulatorConfiguration.hpp) with all the default configurations
      from [Defaults](Defaults.hpp) funneled in
