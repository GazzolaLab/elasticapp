//==============================================================================
/*!
//  \file
//  \brief A data-structure for querying the simulator for specific objects
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

#include <cstdint>

namespace elastica {

  namespace interface {

    namespace detail {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief A QueryID type for a query interface
       * \ingroup interface
       *
       * \details
       * QueryID is a helper class to uniquely identify a
       * constraint/forcing/etc.
       */
      template <typename ModuleParameterTag, typename Type_>
      struct QueryID {
        //**Type definitions****************************************************
        using IndexType = std::size_t;  //!< The index type
        using Type = Type_;  //! The type for conversion in interfaces
        //**********************************************************************

        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The sole constructor.
         *
         * \param id An integer ID to mark the current QueryID
         */
        QueryID(IndexType id) : uuid(id){};
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{

        //**********************************************************************
        /*!\brief Destructor
         */
        ~QueryID() = default;
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Member variables****************************************************
        IndexType const uuid;  //!< Unique integer ID
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //========================================================================
      //
      //  GLOBAL OPERATORS
      //
      //========================================================================

      //************************************************************************
      /*!\brief Returns the QueryID
       *
       * \param uuid The QueryID object.
       * \return Unique index of QueryID object
       */
      template <typename ModuleParameterTag, typename Type_>
      inline auto id(
          QueryID<ModuleParameterTag, Type_> const& query_id) noexcept ->
          typename QueryID<ModuleParameterTag, Type_>::IndexType {
        return query_id.uuid;
      }
      //************************************************************************

      //**Equality operator*****************************************************
      /*!\brief Equality comparison between two QueryID objects.
       *
       * \param lhs The left-hand side uuid.
       * \param rhs The right-hand side uuid.
       * \return \a true if the query ids are same, else \a false
       */
      template <typename ModuleParameterTag, typename Type_>
      inline auto operator==(
          QueryID<ModuleParameterTag, Type_> const& lhs,
          QueryID<ModuleParameterTag, Type_> const& rhs) noexcept -> bool {
        return (id(lhs) == id(rhs));
      }
      //************************************************************************

      //**InEquality operator***************************************************
      /*!\brief InEquality comparison between two QueryID objects.
       *
       * \param lhs The left-hand side uuid.
       * \param rhs The right-hand side uuid.
       * \return \a true if the query ids are not the same, else \a false
       */
      template <typename ModuleParameterTag, typename Type_>
      inline auto operator!=(
          QueryID<ModuleParameterTag, Type_> const& lhs,
          QueryID<ModuleParameterTag, Type_> const& rhs) noexcept -> bool {
        return not(lhs == rhs);
      }
      //************************************************************************

    }  // namespace detail

  }  // namespace interface

}  // namespace elastica