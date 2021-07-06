//==============================================================================
/*!
//  \file
//  \brief Connection ID class
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

#include "Simulator/Interface/Connections/Types.hpp"
#include "Simulator/Interface/Wrappers/UUID.hpp"

namespace elastica {

  namespace interface {

    namespace connections {

      // Developer note : We use it only inside ConnectionBase, so the symbols
      // Tag and UUID are not redefined. In case in the future we have a
      // refactor, move these structs into a CPP file

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Connection ID tag parameter
       * \ingroup connections
       *
       * \details
       * Unique tag parameter for UUID
       */
      struct ConnectionUUIDTag;
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Connection ID
       * \ingroup connections
       *
       * \details
       * ConnectionID is a helper class to uniquely identify a connection
       */
      struct ConnectionUUID
          : public ::elastica::interface::detail::UUID<ConnectionUUIDTag> {
        //**Type definitions****************************************************
        using Parent = ::elastica::interface::detail::UUID<ConnectionUUIDTag>;
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

    }  // namespace connections

  }  // namespace interface

}  // namespace elastica
