# Elastica Examples

This directory contains number of examples demonstrating different features of elastica. All examples are stored in
separate subdirectories, containing case descriptions, cmake configuration, and all other files necessary to run. More
advanced cases are stored in separate repository with its description.

## Example Overview

Some examples provide additional file or link to published paper for a complete description.

### Case studies

* [`axial_stretching`](./axial_stretching)
    * __Name__ : AxialStretchingCaseStudy
    * __Purpose__: Physical convergence test of simple stretching rod.
    * __Features__:
        * CosseratRod
        * Forcing
        * Constraints

* [`timoshenko`](./timoshenko)
    * __Name__ : TimoshenkoCaseStudy
    * __Purpose__: Physical convergence test of simple Timoshenko beam.
    * __Features__:
        * CosseratRod
        * Forcing
        * Constraints

* [`butterfly`](./butterfly)
    * __Name__ : ButterflyCaseStudy
    * __Purpose__: Demonstrate simple restoration with initial strain.
    * __Features__:
        * CosseratRodWithoutDamping
        * Custom initializers

* [`helical_buckling`](./helical_buckling)
    * __Name__ : HelicalBucklingCaseStudy
    * __Purpose__: Demonstrate helical buckling with extreme twisting boundary condition.
    * __Features__:
        * CosseratRod
        * Temporally varying constraints

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

* [`tutorial_casestudy_setup`](./tutorial_casestudy_setup)
    * __Name__: Case study tutorial
    * __Purpose__: Tutorial for setting up a **elastica** case study
    * __Features__:
        * Generating casestudies in `C++` and `python`
        * CaseStudyInterface

### Function

* [`parallelism`](./parallelism)
    * __Name__: Parallelism Example
    * __Name__: Demonstrate shared-memory parallel **elastica** application
        * __Features__:
            * `make_parallel_configuration`
            * `seq`, `par`

* [`display_variables`](./display_variables)
    * __Name__: Display Variable Names
    * __Purpose__: Demonstrate introspection of variables within different Plugins
    * __Features__:
        * `blocks::display_variable_summary`

## Advanced Examples



