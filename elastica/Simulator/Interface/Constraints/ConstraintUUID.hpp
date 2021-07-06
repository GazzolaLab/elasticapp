//==============================================================================
/*!
//  \file
//  \brief Constraint ID class
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
#include <string>
#include <utility>

#include "Simulator/Interface/Constraints/Types.hpp"
#include "Simulator/Interface/Wrappers/UUID.hpp"

namespace elastica {

  namespace interface {

    namespace constraints {

      // Developer note : We use it only inside ConstraintBase, so the symbols
      // Tag and UUID are not redefined. In case in the future we have a
      // refactor, move these structs into a CPP file

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Constraint ID tag parameter
      // \ingroup constraints
      //
      // \details
      // Unique tag parameter for UUID
      */
      struct ConstraintUUIDTag;
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Constraint ID
      // \ingroup constraints
      //
      // ConstraintID is a helper class to uniquely identify a constraint
      */
      struct ConstraintUUID
          : public ::elastica::interface::detail::UUID<ConstraintUUIDTag> {
        //**Type definitions****************************************************
        using Parent = ::elastica::interface::detail::UUID<ConstraintUUIDTag>;
        //**********************************************************************

        //**Constructors********************************************************
        /*!\name Constructors */
        //@{
        using Parent::Parent;
        using Parent::operator=;
        //@}
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace constraints

  }  // namespace interface

}  // namespace elastica
