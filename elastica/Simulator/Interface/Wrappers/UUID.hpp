//==============================================================================
/*!
//  \file
//  \brief A common UUID data-structure
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
#include <functional>
#include <limits>
#include <ostream>
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
        using IndexType = std::size_t;    //!< The index type
        using TagType = ParameterTag;     //!< The tag type
        using This = UUID<ParameterTag>;  //!< This type
        //**********************************************************************

        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief default constructor for map initialization
         */
        UUID() : uuid(std::numeric_limits<IndexType>::max()), name() {}
        //**********************************************************************

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
        // UUID(UUID&& other) noexcept = default;
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
       * \return \a true if the uuids are not the same, else \a false
       */
      template <class ParameterTag>
      bool operator!=(UUID<ParameterTag> const& lhs,
                      UUID<ParameterTag> const& rhs) noexcept {
        return not(lhs == rhs);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Global output operator for UUID
       *
       * \param os Reference to the output stream.
       * \param uuid Reference to an axis-aligned bounding box.
       * \return Reference to the output stream.
       */
      template <typename ParameterTag>
      inline std::ostream& operator<<(std::ostream& os,
                                      UUID<ParameterTag> const& uuid) {
        return os << "(" << id(uuid) << ", " << name(uuid) << ")";
      }
      //************************************************************************

    }  // namespace detail

  }  // namespace interface

}  // namespace elastica

namespace std {

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  template <typename ParameterTag>
  struct hash< ::elastica::interface::detail::UUID<ParameterTag> > {
    using T = ::elastica::interface::detail::UUID<ParameterTag>;
    inline auto operator()(const T& t) const -> std::size_t {
      return hash<typename T::IndexType>{}(
          ::elastica::interface::detail::id(t));
    }
  };
  /*! \endcond */
  //****************************************************************************

}  // namespace std