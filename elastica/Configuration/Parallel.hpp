//==============================================================================
/*!
//  \file
//  \brief Configuration of the parallelization library
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Compilation switch for the (de-)activation of the shared-memory
// parallelization (ELASTICA_USE_SHARED_MEMORY_PARALLELIZATION). When set to 1,
// Elastica++ may run operations in parallel; when 0, parallel execution is
// disabled. Activate via -D on the compiler command line or before including
// Blaze headers.
// #ifndef ELASTICA_USE_SHARED_MEMORY_PARALLELIZATION
// #define ELASTICA_USE_SHARED_MEMORY_PARALLELIZATION @ELASTICA_SMP@
// #endif
//******************************************************************************

//******************************************************************************
/*!\brief Compile time hint for number of threads to be used in an elastica
 * application
 * \ingroup parallel
 */
constexpr std::size_t elastica_threads_hint() { return 2UL; }
//******************************************************************************

//******************************************************************************
/*!\brief Environment variable to set a hint for the number of threads used
 * \ingroup parallel
 *
 * The macro defines the environment variable used to provide hint for the
 * number of threads to set for Elastica++ applications.
 * For user applications (i.e. when writing your own main-file), this variable
 * does not factor in : rather the user can set the number of threads they want
 * using the specific parallelism library they employed.
 */
#define ENV_ELASTICA_NUM_THREADS "ELASTICA_NUM_THREADS"
  //****************************************************************************
