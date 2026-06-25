\mainpage Introduction

# What is Elastica++? {#intro_sec}

[TOC]

> [!note]
> If you are new to the elastica project, we recommend to first look into python-implementation [PyElastica](https://github.com/GazzolaLab/PyElastica) to experience Cosserat-rod simulation and its applications. **Elastica++** is an optimized solver for **problems at scale**. In small problems where compute is CPU-bounded, this CPP implementation may not provide much advantages.

`Elastica++` is a `C++` package to simulate the dynamics of soft-filaments that, at every cross-section, can undergo all six possible modes of deformation, allowing the filament to bend, twist, stretch and shear. Such slender structures are ubiquitous in natural and artificial systems across scales, from DNA strands to flagella and space tethers. They also make up a variety of complex systems, from birds’ nests made of twigs to living creatures made of bones, tendons, fibers and muscles.

`Elastica++` models these filaments using Cosserat rod theory which allows for modeling assemblies of such rods while interacting with complex exogenous (surface contact, friction and hydrodynamics) and endogenous (muscular activity) effects. `Elastica++` maps such assemblies efficiently to multicore processors using a novel Block architecture which allows it to scale from laptops to computing clusters. More information about `Elastica++` is available at the Elastica [project website](https://cosseratrods.org)

# Getting started {#getting_started_sec}

For build and installation guidance, start with the \ref installation page.

# Learn the codebase {#learn_sec}

Use the narrative guides to understand the core abstractions before diving into
the API reference:

- \ref core_concepts_page "Core Concepts"

# Build your own simulation {#build_your_own_sec}

If your goal is to adapt Elastica++ for personal use, start here:

- \ref examples_page "Case Examples"

# Case Examples {#examples_sec}

Start with the curated \ref examples_page "Case Examples" page for grouped example programs and
recommended entry points. Use the examples as runnable companions to the
tutorial and setup guides rather than as a flat catalog.

# Roadmap {#roadmap_sec}

For outstanding architectural and implementation work, see the
\ref todo_page "Project TODO".

# Documentation map {#documentation_map_sec}

The generated documentation is organized around the following entry points:

- Learn: \ref installation "Installation",
  \ref core_concepts_page "Core Concepts"
- Browse runnable examples: \ref examples_page "Case Examples"
- API reference:
- \ref todo_page "Project TODO"
- \ref systems "Physical systems"
- \ref elastica::cosserat_rod "Cosserat rods"
- \ref rigid_body "Rigid bodies"
- \ref blocks "Blocks"
- \ref system_tags "System tags"

For API browsing, use the navigation tabs above.
