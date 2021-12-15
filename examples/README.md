# Elastica Examples

This directory contains number of examples demonstrating different features of elastica. All examples are stored in
separate subdirectories, containing case descriptions, cmake configuration, and all other files necessary to run. More
advanced cases are stored in separate repository with its description.

## Example Overview

Some examples provide additional file or link to published paper for a complete description.

### Rod Assembly

* [`tutorial_single_rod`](./tutorial_single_rod)
    * __Name__: Single rod tutorial
    * __Purpose__: Demonstrate API afforded by an __elastica__ Cosserat rod
    * __Features__:
        * Cosserat Rod API
        * elastica::get
        * Tags
        * Accessors

* [`tutorial_rod_construction`](./tutorial_rod_construction)
    * __Name__: Rod construction tutorial
    * __Purpose__: Demonstrate creating a single rod in the context of an elastica simulation.
    * __Features__:
        * Cosserat Rod
        * Initializer
        * blocks::initialize
        * Material

### Function

* [`parallelism`](./parallelism)
    * __Purpose__: Parallelism Example
    * __Features__:
* [`timestep_rod`](./timestep_rod)
    * __Name__: Timestep Rod
    * __Purpose__: Demonstration of time-stepper (symplectic stepper) configuration using single-rod case.
    * __Features__:
        * Symplectic Stepper
        * Simulator
        * System Configuration

### Miscellaneous

* [`display_variables`](./display_variables)
    * __Name__: Display Variable Names
    * __Purpose__:
    * __Features__:

## Advanced Examples



