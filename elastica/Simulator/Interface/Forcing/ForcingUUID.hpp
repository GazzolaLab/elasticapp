//==============================================================================
/*!
//  \file
//  \brief Forcing ID class
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

#include "Simulator/Interface/Forcing/Types.hpp"
#include "Simulator/Interface/Wrappers/QueryID.hpp"
#include "Simulator/Interface/Wrappers/UUID.hpp"

namespace elastica {

  namespace interface {

    namespace forcing {

      // Developer note : We use it only inside ForcingBase, so the symbols
      // Tag and UUID are not redefined. In case in the future we have a
      // refactor, move these structs into a CPP file

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Forcing ID tag parameter
      // \ingroup forcing
      //
      // \details
      // Unique tag parameter for UUID
      */
      struct ForcingIDTag;
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Forcing ID
      // \ingroup forcing
      //
      // ForcingID is a helper class to uniquely identify a forcing
      */
      struct ForcingUUID
          : public ::elastica::interface::detail::UUID<ForcingIDTag> {
        //**Type definitions****************************************************
        using Parent = ::elastica::interface::detail::UUID<ForcingIDTag>;
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

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Forcing ID
       * \ingroup connections
       *
       * \details
       * ForcingID is a helper class to uniquely identify a connection
       */
      template <typename Type_>
      struct ForcingQueryID
          : public ::elastica::interface::detail::QueryID<ForcingIDTag, Type_> {
        //**Type definitions****************************************************
        using Parent =
            ::elastica::interface::detail::QueryID<ForcingIDTag, Type_>;
        //**********************************************************************

        //**Constructors********************************************************
        /*!\name Constructors */
        //@{
        using Parent::Parent;
        //@}
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace forcing

  }  // namespace interface

}  // namespace elastica
