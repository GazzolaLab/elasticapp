\page core_concepts_page Core Concepts

[TOC]

This page introduces the main ideas you need before modifying or creating a
simulation.

## Simulator and configuration

An Elastica++ simulation is assembled around a simulator object plus a
configuration that determines:

- which systems are allowed
- how systems are grouped into blocks
- how iteration is performed
- which domain, parallel, and module-level settings are active

The basic pattern looks like this:

\code{.cpp}
using ::elastica::cosserat_rod::CosseratRod;
namespace ec = elastica::configuration;

auto system_config = ec::make_systems_configuration<tmpl::list<CosseratRod>>(
    ec::make_blocking_policy(ec::RestrictSizeAcrossBlockTypes{50UL}),
    ec::make_iteration_policy(ec::iterate_across_system_types_in(ec::seq),
                              ec::iterate_over_systems_in(ec::seq)));

auto simulator_config = ec::make_simulator_configuration(
    ec::DefaultDomainConfiguration{},
    system_config,
    ec::DefaultParallelConfiguration{});

auto simulator = elastica::make_simulator<>(simulator_config);
\endcode

Start with:

- `elastica::make_simulator`
- simulator configuration helpers in the `configuration` namespace

## Systems and plugins

The systems module defines the physical entities that participate in the
simulation, such as Cosserat rods and rigid bodies.

Important ideas:

- a simulation declares which system types it will contain
- systems are added to the simulator and later finalized
- plugins, tags, and traits shape what data and behavior a system exposes

For most first simulations, the main starting point is
\ref elastica::cosserat_rod.

A minimal system-emplacement pattern looks like:

\code{.cpp}
auto rod_ref = simulator->emplace_back<CosseratRod>(
    straight_initializer.initialize<CosseratRod>());
\endcode

The returned value is a wrapped reference, which lets you safely keep access to
the created system.

## Rods, materials, initializers, tags, and blocks

Cosserat rods are the most common entry point for new users.

The typical flow is:

1. create or choose a material
2. choose an initializer such as a straight-rod initializer
3. initialize the rod state
4. add the rod to the simulator
5. access rod data through member functions, free functions, or tags

Material and initializer setup usually looks like:

\code{.cpp}
elastica::MaterialID material = elastica::create_material(
    7.874, 0.24, 200, 200.0 / 1.24, 200, 1e-10, 1e-10, 0.1, 0.1);

using StraightInit = ::elastica::cosserat_rod::StraightCosseratRodInitializer;
StraightInit initializer{
    StraightInit::NElement{10UL},
    StraightInit::Material{material},
    StraightInit::Radius{0.007522},
    StraightInit::Length{0.18},
    StraightInit::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
    StraightInit::Direction{::elastica::Vec3{0.0, 1.0, 0.0}},
    StraightInit::Normal{::elastica::Vec3{0.0, 0.0, 1.0}},
    StraightInit::ForceDampingRate{0.2},
    StraightInit::TorqueDampingRate{0.2},
};
\endcode

Optional initialization can be overridden at construction time:

\code{.cpp}
simulator->emplace_back<CosseratRod>(
    initializer.initialize<CosseratRod>(
        ::blocks::initialize<::elastica::tags::Velocity>(
            [](std::size_t index) {
              return 0.1 + 0.002 * static_cast<double>(index);
            })));
\endcode

The tutorial examples show three access patterns for rod state:

\code{.cpp}
auto &rod = rod_ref.get();

auto member_position = rod.get_position();
auto free_position = ::elastica::position(rod);
auto tagged_position = ::elastica::get<::elastica::tags::Position>(rod);
\endcode

Blocks are the execution and storage grouping mechanism used by the simulator.
They become more important when you start reasoning about performance,
parallelism, and multi-system simulations.

## Iteration and time stepping

Time stepping drives the simulation forward.

When reading or writing simulation code, keep these concepts separate:

- current simulation state
- the stepper or time integration algorithm
- callbacks or callables run at each step
- stopping conditions such as total time or iteration count

The case-study tutorials introduce the split between setup, run, and step:

\code{.cpp}
void setup();
void run();
void step(const double time, const double dt);
\endcode

For onboarding, the important point is not every stepping detail but the fact
that simulation setup and simulation advancement are distinct phases.

## Environments, constraints, connections, and IO

Most personal simulations are built by starting with a rod or rigid body and
then layering on behavior:

- environments for effects such as gravity or collision context
- constraints for fixed or time-varying restrictions
- connections or joints for linked systems
- IO for metadata, HDF5, YAML, CSV, or restart workflows

The practical workflow usually looks like:

\code{.cpp}
// 1. Create simulator
auto simulator = elastica::make_simulator<>(simulator_config);

// 2. Add systems
auto rod_ref = simulator->emplace_back<CosseratRod>(
    initializer.initialize<CosseratRod>());

// 3. Layer in extra behavior in the style of the closest reference example
//    (constraints, forcing, joints, environments, IO)

// 4. Finalize and run
\endcode

When choosing an example to copy from, it is usually best to choose based on
which of these behaviors you need first.

## Suggested next reading

- \ref simulation_setup_page "Simulation Setup Guide"
- \ref examples_page "Case Examples"
