# Elastica Examples

This directory contains number of examples demonstrating different features of elastica. All examples are stored in
separate subdirectories, containing case descriptions, cmake configuration, and all other files necessary to run. More
advanced cases are stored in separate repository with its description.

## Example Overview

Some examples provide additional file or link to published paper for a complete description.

### Case studies

* [`axial_stretching`](./axial_stretching)<img src="https://img.shields.io/badge/python-Case%20Study-brightgreen"/>
    * __Name__ : AxialStretchingCaseStudy
    * __Purpose__: Physical convergence test of simple stretching rod.
    * __Features__:
        * CosseratRod
        * Forcing
        * Constraints

* [`butterfly`](./butterfly)<img src="https://img.shields.io/badge/python-Case%20Study-brightgreen"/>
    * __Name__ : ButterflyCaseStudy
    * __Purpose__: Demonstrate simple restoration with initial strain.
    * __Features__:
        * CosseratRodWithoutDamping
        * Customizing simulator with a user-defined configuration
        * Custom initializers

* [`timoshenko`](./timoshenko)<img src="https://img.shields.io/badge/python-Case%20Study-brightgreen"/>
    * __Name__ : TimoshenkoCaseStudy
    * __Purpose__: Physical convergence test of simple Timoshenko beam.
    * __Features__:
        * CosseratRod
        * Forcing
        * Constraints
        * EnableReset

* [`helical_buckling`](./helical_buckling)<img src="https://img.shields.io/badge/python-Case%20Study-brightgreen"/>
    * __Name__ : HelicalBucklingCaseStudy
    * __Purpose__: Demonstrate helical buckling with extreme twisting boundary condition.
    * __Features__:
        * CosseratRod
        * Temporally varying constraints

* [`hinge_joint`](./hinge_joint)<img src="https://img.shields.io/badge/python-Case%20Study-brightgreen"/>
    * __Name__ : HingeJointCaseStudy
    * __Purpose__: Demonstrate connecting two rods using joints
    * __Features__:
        * CosseratRod
        * Temporally varying Connections between rods
        * Forcing

* [`flexible_swinging_pendulum`](./flexible_swinging_pendulum)<img src="https://img.shields.io/badge/python-Case%20Study-brightgreen"/>
    * __Name__ : FlexibleSwingingPendulumCaseStudy
    * __Purpose__: Complex oscillatory dynamics of a flexible pendulum, swinging under the action of gravity.
    * __Features__:
        * CosseratRod
        * Constraints
        * Environments (GravityField)

* [`rigid_body_collision`](./rigid_body_collision)<img src="https://img.shields.io/badge/python-Case%20Study-brightgreen"/>
    * __Name__ : RigidBodyCollisionCaseStudy
    * __Purpose__: Modeling rigid body dynamics and collision forces.
    * __Features__:
        * Sphere (RigidBody)
        * Environments (CollisionSystem)

* [`rod_collision`](./rod_collision)<img src="https://img.shields.io/badge/python-Case%20Study-brightgreen"/>
    * __Name__ : RodCollisionCaseStudy
    * __Purpose__: Modeling collision forces between moving elastic rods.
    * __Features__:
        * CosseratRod
        * Environments (CollisionSystem)

* [`imposed_primitives`](./imposed_primitives)<img src="https://img.shields.io/badge/python-Case%20Study-brightgreen"/>
    * __Name__ : ImposedPrimitivesCaseStudy
    * __Purpose__: Imposing oscillatory motion of primitives (plane) interacting with other objects (sphere)
    * __Features__:
        * Sphere (RigidBody)
        * ImposedPlane (RigidBody)
        * Translations (Kinematics)
        * Environments (CollisionSystem, GravityField)

* [`rod_plane_collision`](./rod_plane_collision)<img src="https://img.shields.io/badge/python-Case%20Study-brightgreen"/>
    * __Name__ : RodPlaneCollisionCaseStudy
    * __Purpose__: Imposing oscillatory motion of primitives (plane) interacting with rods
    * __Features__:
        * CosseratRod
        * ImposedPlane (RigidBody)
        * Translations (Kinematics)
        * Environments (CollisionSystem, GravityField)

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

* [`tutorial_casestudy_setup`](./tutorial_casestudy_setup)<img src="https://img.shields.io/badge/python-Case%20Study-brightgreen"/>
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

* [`simulation_reset`](./simulation_reset)
    * __Name__: Reset simulation
    * __Purpose__: Demonstrate a method to reset a simulation, intended for independent, distributed **elastica++**
      applications
    * __Features__:
        * `elastica::modules::EnableReset`

## Advanced Examples



