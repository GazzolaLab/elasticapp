\page examples_page Case Examples

This page is a short chooser for the case-study and tutorial example tree.

For the complete automatically generated example list, use the `Examples` tab
in the Doxygen navigation. For a source-tree overview, see
[examples/README.md](../examples/README.md).

If you are still onboarding, start with the
\ref simulation_setup_page "Simulation Setup Guide" first.

## Best first choices

- [tutorial_single_rod.cpp](../examples/tutorial_single_rod/tutorial_single_rod.cpp)
  Read rod state, tags, and array shapes.
- [tutorial_rod_construction.cpp](../examples/tutorial_rod_construction/tutorial_rod_construction.cpp)
  Build and initialize a rod inside a simulator.
- [tutorial_casestudy_setup](../examples/tutorial_casestudy_setup/CMakeLists.txt)
  Learn the case-study structure for reusable simulations.
- [AxialStretchingCaseStudy.cpp](../examples/axial_stretching/AxialStretchingCaseStudy.cpp)
  Good first parameterized personal simulation.

## Choose by need

- Need a single-rod starting point:
  [tutorial_rod_construction.cpp](../examples/tutorial_rod_construction/tutorial_rod_construction.cpp)
- Need joints or coupled systems:
  [HingeJointCaseStudy.cpp](../examples/hinge_joint/HingeJointCaseStudy.cpp)
- Need collision/contact:
  [RodCollisionCaseStudy.cpp](../examples/rod_collision/CaseStudy/RodCollisionCaseStudy.cpp)
  or [RodPlaneCollisionCaseStudy.cpp](../examples/rod_plane_collision/plane/RodPlaneCollisionCaseStudy.cpp)
- Need parallel or runtime tuning:
  [examples/parallelism/main.cpp](../examples/parallelism/main.cpp)
  or [parallel_butterfly/main.cpp](../examples/parallel_butterfly/main.cpp)

## Advanced collections

- [examples/active_matter](../examples/active_matter/CMakeLists.txt)
- [examples/magnetism](../examples/magnetism/README.md)
- [examples/nest](../examples/nest/CMakeLists.txt)

Treat these as reference collections after you already have a working base
simulation.
