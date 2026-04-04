//==============================================================================
/*!
//  \file
//  \brief The default traits class for Cosserat Rods within \elastica
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <type_traits>

// Elastica
#include "Systems/Block/Block.hpp"  // variables
#include "Systems/RigidBody/Traits/DataOpsTraits.hpp"
#include "Systems/RigidBody/Types.hpp"
//
#include "Utilities/NonCreatable.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

namespace elastica {

  namespace rigid_body {

    //==========================================================================
    //
    //  CLASS DEFINITIONS
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Traits class collecting common meta-functions for implementing
     * Cosserat Rods
     * \ingroup rigid_body
     *
     * \details
     * RigidBodyTraits offers a compile-time customization point for the data
     * -structures, placement and algorithms needed to implement a Rigid body
     * hierarchy implemented using @ref blocks in \elastica. RigidBodyTraits
     * is templated on three types controlling orthogonal aspects of a Cosserat
     * Rod---the data structures used, where these are placed in the
     * finite-difference grids (similar to primal and dual grids, for example
     * on nodes/elements/voronoi) and memory allocation. This makes it possible
     * to customize the Traits class if needed.
     *
     * Finally, an instantiation of RigidBodyTraits is used as the first
     * template parameter in elastica::rigid_body::RigidBodyPlugin and
     * across all components defined in @ref rigid_body_component.
     * It does not need customization, but
     *
     * \tparam TraitsForData Traits controlling the data-structures
     * \tparam TraitsForPlacement Traits controlling the placement of variables
     * \tparam TraitsForAllocation Traits controlling memory allocation
     */
    template <typename TraitsForDataOps, typename TraitsForAllocation>
    struct RigidBodyTraits : private NonCreatable {
      //**Type definitions******************************************************
      //! Data type for seamless use in components
      using DataType = TraitsForDataOps;
      //! Real type
      using real_type = typename DataType::real_type;
      //! Type of index
      using index_type = typename DataType::index_type;
      //! Type of size
      using size_type = std::size_t;
      //! Operations type to act on the data, typically contains kernels
      using Operations = typename TraitsForDataOps::Operations;
      //************************************************************************

      //************************************************************************
      /*!\brief Variable for use in @ref rigid_body_component
       *
       * \details
       * RigidBodyVariable is a special blocks::Variable customized to the
       * need of cosserat rods
       *
       * \tparam ParameterTag A unique type parameter to mark the Variable with
       * \tparam RankTag      Type for customizing the data implementation, from
       * DataType
       * \tparam Tags...      Other types to provide customization by
       * inheritance
       *
       * \see blocks::Variable
       */
      template <typename ParameterTag, typename RankTag, typename... Tags>
      struct RigidBodyVariable
          : public ::blocks::Variable<ParameterTag, RankTag, Tags...> {};
      //************************************************************************

      //************************************************************************
      /*!\brief InitializedVariable for use in @ref rigid_body_component
       *
       * \details
       * RigidBodyInitializedVariable is a special blocks::InitializedVariable
       * customized to the need of cosserat rods
       *
       * \tparam ParameterTag A unique type parameter to mark the
       * InitializedVariable with
       * \tparam RankTag      Type for customizing the data implementation, from
       * DataType
       * \tparam Tags... Other types to provide customization by inheritance
       *
       * \see blocks::InitializedVariable
       */
      template <typename ParameterTag, typename RankTag, typename... Tags>
      struct RigidBodyInitializedVariable
          : public ::blocks::InitializedVariable<ParameterTag, RankTag,
                                                 Tags...> {};
      //************************************************************************
    };
    //**************************************************************************

    //**************************************************************************
    /*!\brief Default traits for Cosserat Rods
     * \ingroup rigid_body
     *
     * \see RigidBodyTraits
     */
    struct DefaultRigidBodyTraits
        : public RigidBodyTraits<DataOpsTraits, void> {};
    //**************************************************************************

  }  // namespace rigid_body

}  // namespace elastica
