\page todo_page Project TODO

[TOC]

This page tracks longer-term technical work that still appears to be open.
Items that already have clear implementations in the current codebase, such as
time-stepping infrastructure, serialization support, reset support, metadata
introspection, and existing collision backends, have been removed from this
list.

## Bugs To Be Fixed

- Review open correctness and stability issues discovered during active
  development.

## Memory And Data Layout

- Revisit block memory kernels for dimension- and system-specific layouts.
- Explore fixed-span block variants with tunable maximum size.
- Explore free-span block variants better suited for BVH-style structures.
- Evaluate movable versus non-movable block storage for spatial hashing and
  related data structures.
- Decide whether a global vector of blocks or a different aggregate layout is
  the right long-term container strategy.

## Space Adaptation

- Add a space-adaptation interface that can operate on aggregates and their
  history/state data together.
- Introduce block creation, deletion, split, and merge policies for adaptive
  storage.
- Investigate Eulerian tree-based adaptation for fixed blocks.
- Investigate Lagrangian BVH-style adaptation for free blocks.
- Study combined space-time reordering, including occasional memory reordering
  for hierarchy- and contact-aware access patterns.

## Time Integration

- Add a timestep adapter and timestep chooser strategy for more flexible
  integration control.

## Application UX

- Design rendering/output customization points with sensible defaults and
  object-specific specializations.
- Add a callback model that supports both stateful functors and lightweight
  stateless lambdas.

## Parallelism And Performance

- Investigate dynamic SIMD backends such as ISPC where they provide a real
  benefit.
- Add a NUMA-aware execution/storage layer if the workload justifies it.
- Evaluate whether selective SoA/AoS transformations would improve collision
  processing performance.

## Collision Detection

- Add a sweep-and-prune broad-phase strategy.
- Add callback hooks for features such as reference counting or time-of-impact
  estimation.
