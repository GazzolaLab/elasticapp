//==============================================================================
/*!
//  \file
//  \brief Types of configuration for the modules in \elastica
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

namespace elastica {

  namespace configuration {

    ////////////////////////////////////////////////////////////////////////////
    //
    // Forward declarations of configuration types
    //
    ////////////////////////////////////////////////////////////////////////////

    //****************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /// details, to nclude exexcution?
    template <typename Criterion>
    struct BlockingPolicy;
    template <typename ExecutionPolicyOverSystemTypes,
              typename ExecutionPolicyForEachSystemType, typename CriteriaIfAny>
    struct IterationPolicy;
    ///
    template <typename ExecutionPolicy, typename Registrars>
    class ConnectionsConfiguration;
    template <typename ExecutionPolicy, typename Registrars>
    class ConstraintsConfiguration;
    template <typename BoundaryConditionInX, typename BoundaryConditionInY,
              typename BoundaryConditionInZ>
    class DomainConfiguration;
    template <typename EnvironmentTags, typename ImposedPrimitives,
              typename ExecutionPolicy>
    class EnvironmentsConfiguration;
    template <typename ExecutionPolicy, typename Registrars>
    class ForcingConfiguration;
    template <typename SystemPlugins, typename BlockingStrategy,
              typename IterationStrategy>
    class SystemsConfiguration;
    /*! \endcond */
    //****************************************************************************

  }  // namespace configuration

}  // namespace elastica