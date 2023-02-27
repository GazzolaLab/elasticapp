//==============================================================================
/*!
//  \file
//  \brief Datatraits for Rigid bodies within \elastica
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <cstddef>
///
#include "Systems/RigidBody/Traits/Types.hpp"
#include "Utilities/DefineTypes.h"
///
//#include "Systems/RigidBody/Traits/DataTraits/Rank.hpp"
// Not used, since we directly use blaze's implementation of a vector
//#include "Systems/RigidBody/Traits/DataType/ScalarTag.hpp"
///

#include "Systems/common/Traits/DataType/BlazeBackend/DataTypeTraits.hpp"
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/OpsTraits.hpp"
#include "Systems/CosseratRods/Traits/Operations/BlazeBackend/OptimizationLevel.hpp"
///
#include "Utilities/NonCreatable.hpp"

namespace elastica {

  namespace rigid_body {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Traits implementing data-structures for Rigid bodys in \elastica
     * \ingroup rigid_body
     *
     * \details
     * DataOpsTraits is the customization point for altering the data-structures
     * to be used within the Rigid body hierarchy implemented using @ref blocks
     * in \elastica. It defines (domain-specific) types corresponding to a
     * Rigid body (such as a matrix, vector etc.), and is intended for use as
     * a template parameter in RigidBodyTraits.
     *
     * \see elastica::rigid_body::RigidBodyTraits
     */
    struct DataOpsTraits : private NonCreatable {
      //**Type definitions******************************************************
      //! Real type
      using real_type = real_t;
      //! Type of index
      using index_type = std::size_t;
      // using OnRod = ScalarTag<real_type>;
      //! Type to track indices on a Rigid body, with shape (1, )
      using Index = ::elastica::cosserat_rod::VectorTag<index_type>;
      //! Type to track scalars on a Rigid body, with shape (1, )
      using Scalar = ::elastica::cosserat_rod::VectorTag<real_type>;
      //! Type to track vectors on a Rigid body, with shape (d)
      using Vector = ::elastica::cosserat_rod::MatrixTag<real_type>;
      //! Type to track matrices on a Rigid body, with shape (d, d)
      using Matrix = ::elastica::cosserat_rod::TensorTag<real_type>;
      //************************************************************************

      //**Operation definitions*************************************************
      using Operations = ::elastica::cosserat_rod::OpsTraits<
          ::elastica::cosserat_rod::OptimizationLevel::basic>;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace rigid_body

}  // namespace elastica
