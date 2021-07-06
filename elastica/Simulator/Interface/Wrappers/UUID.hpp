//==============================================================================
/*!
//  \file
//  \brief A common UUID data-structure
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
      /*!\brief A common UUID type for the interface
       * \ingroup interface
       *
       * \details
       * UUID is a helper class to uniquely identify a constraint/forcing/etc.
       */
      template <typename ParameterTag>
      struct UUID {
        //**Type definitions****************************************************
        using IndexType = std::size_t;  //!< The index type
        using TagType = ParameterTag;   //!< The tag type
        //**********************************************************************

        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The sole constructor.
         *
         * \param id An integer ID to mark the current UUID
         * \param aname A Name for marking the current UUID
         */
        UUID(IndexType id, std::string aname)
            : uuid(id), name(std::move(aname)){};
        //**********************************************************************

        //**********************************************************************
        /*!\brief The copy constructor.
         */
        UUID(UUID const& other) = default;
        //**********************************************************************

        //**********************************************************************
        /*!\brief The move constructor.
         */
        UUID(UUID&& other) noexcept = default;
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{

        //**********************************************************************
        /*!\brief Destructor
         */
        ~UUID() = default;
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Member variables****************************************************
        IndexType uuid;    //!< Unique integer ID
        std::string name;  //!< Name, if given
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
      /*!\brief Returns the UUID
       *
       * \param uuid The UUID object.
       * \return Unique index of UUID object
       */
      template <class ParameterTag>
      inline auto id(UUID<ParameterTag> const& uuid) noexcept ->
          typename UUID<ParameterTag>::IndexType {
        return uuid.uuid;
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Returns the UUID
       *
       * \param uuid The UUID object.
       * \return (Optional) name of the UUID representation
       */
      template <class ParameterTag>
      inline auto name(UUID<ParameterTag> const& uuid) noexcept -> std::string {
        return uuid.name;
      }
      //************************************************************************

      //**Equality operator*****************************************************
      /*!\brief Equality comparison between two UUID objects.
       *
       * \param lhs The left-hand side uuid.
       * \param rhs The right-hand side uuid.
       * \return \a true if the uuids are same, else \a false
       */
      template <class ParameterTag>
      inline auto operator==(UUID<ParameterTag> const& lhs,
                             UUID<ParameterTag> const& rhs) noexcept -> bool {
        return (id(lhs) == id(rhs)) && (name(lhs) == name(rhs));
      }
      //************************************************************************

      //**InEquality operator***************************************************
      /*!\brief InEquality comparison between two UUID objects.
       *
       * \param lhs The left-hand side uuid.
       * \param rhs The right-hand side uuid.
       * \return \a true if the uuids are same, else \a false
       */
      template <class ParameterTag>
      bool operator!=(UUID<ParameterTag> const& lhs,
                      UUID<ParameterTag> const& rhs) noexcept {
        return not(lhs == rhs);
      }
      //************************************************************************

    }  // namespace detail

  }  // namespace interface

}  // namespace elastica
