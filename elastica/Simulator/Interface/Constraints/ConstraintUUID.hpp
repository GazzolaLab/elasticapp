//==============================================================================
/*!
//  \file
//  \brief Constraint ID class
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
#include <string>
#include <utility>

#include "Simulator/Interface/Constraints/Types.hpp"
#include "Simulator/Interface/Wrappers/QueryID.hpp"
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
      struct ConstraintIDTag;
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
          : public ::elastica::interface::detail::UUID<ConstraintIDTag> {
        //**Type definitions****************************************************
        using Parent = ::elastica::interface::detail::UUID<ConstraintIDTag>;
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
      /*!\brief Constraint ID
       * \ingroup connections
       *
       * \details
       * ConstraintID is a helper class to uniquely identify a connection
       */
      template <typename Type_>
      struct ConstraintQueryID
          : public ::elastica::interface::detail::QueryID<ConstraintUUID,
                                                          Type_> {
        //**Type definitions****************************************************
        using Parent =
            ::elastica::interface::detail::QueryID<ConstraintUUID, Type_>;
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

    }  // namespace constraints

  }  // namespace interface

}  // namespace elastica

namespace std {

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <>
  struct hash< ::elastica::interface::constraints::ConstraintUUID> {
    using T = ::elastica::interface::constraints::ConstraintUUID;
    inline auto operator()(const T& t) const -> std::size_t {
      using Parent = typename T::Parent;
      return hash<typename T::Parent>{}(static_cast<Parent const&>(t));
    }
  };
  /*! \endcond */
  //****************************************************************************

}  // namespace std
