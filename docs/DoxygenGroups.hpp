//==============================================================================
/*!
 * \file DoxygenGroups.hpp
 * \brief Central Doxygen module groups (documentation only; not compiled).
 *
 * These definitions register module group IDs used with the ingroup command
 * across the library. The docs directory is listed first in Doxygen INPUT so
 * groups here are registered early.
 */
//==============================================================================

#pragma once

//******************************************************************************
/*!\defgroup utils Utilities
 * \brief Umbrella for general-purpose helpers (geometry, logging, traits, etc.)
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup util Low-level utilities
 * \ingroup utils
 * \brief Small helpers (memory, CRTP, non-copyable, etc.)
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup utilities Utilities (alternate id)
 * \ingroup utils
 * \brief Alternate spelling used by some headers.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup UtilitiesGroup Utilities (legacy name)
 * \ingroup utils
 * \brief Legacy module id; prefer \ref utils where possible.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup type_traits Type traits
 * \ingroup utils
 * \brief Type traits and metaprogramming helpers (alongside \c type_traits_group).
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup interface Simulator interfaces
 * \ingroup simulator
 * \brief Abstract interfaces to the simulator core.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup io Input and output
 * \ingroup simulator
 * \brief File, HDF5, YAML, and related I/O interfaces.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup environment Environment (singular)
 * \ingroup environments
 * \brief Supplemental group id for headers that use the singular name alongside
 *        the main environments module.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup customization Customization hooks
 * \ingroup environments
 * \brief Transfer and primitive customization traits.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup core Collision / DEM core
 * \ingroup collision_system
 * \brief Core collision and DEM solver documentation group.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup rigid_body RigidBody
 * \ingroup systems
 * \brief Data, operations and helpers for manipulating rigid bodies.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup rigid_body_component Rigid body components
 * \ingroup rigid_body
 * \brief Geometry, kinematics, and related rigid-body components.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup OptionParsingGroup Command-line options
 * \ingroup modules
 * \brief Option parsing and parameters.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup ErrorHandlingGroup Error handling
 * \brief Assertions, errors, and contract checking.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup TimeSteppersGroup Time stepping
 * \ingroup time
 * \brief History buffers and stepper-related helpers.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup symplectic Symplectic integrators
 * \ingroup time
 * \brief Symplectic time integration algorithms.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup math Math helpers
 * \ingroup utils
 * \brief Vector length, normalize, and similar math utilities.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup math_shims Math shims
 * \ingroup math
 * \brief Thin wrappers around math operations.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup fine_detection Fine collision kernels
 * \ingroup fine_collision_detection
 * \brief Narrow-phase collision detection helpers.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup cosserat_rods Cosserat rods (plural id)
 * \ingroup cosserat_rod
 * \brief Legacy group id; see \ref cosserat_rod.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup vtk VTK output
 * \ingroup serialization
 * \brief VTK writers and related I/O.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup extension_environments Example environment extensions
 * \brief Field profiles and extensions used by example programs.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup extension Example extensions
 * \brief Adapters and helpers for example-specific code.
 */
//******************************************************************************

//******************************************************************************
/*!\defgroup joints Joints
 * \ingroup simulator
 * \brief Interfaces for coupled forces and torques between systems.
 */
//******************************************************************************
