//==============================================================================
/*!
//  \file
//  \brief Connection base class
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
#include <memory>
#include <string>

///
#include "Simulator/Interface/Connections/Types.hpp"
///
#include "Simulator/Connections/Connection.hpp"
#include "Simulator/Connections/Types.hpp"
#include "Simulator/Interface/Connections/ConnecterBase.hpp"
#include "Simulator/Interface/Connections/ConnectionUUID.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/NumericAt.hpp"

namespace elastica {

  namespace interface {

    namespace connections {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Base class for Connection
       * \ingroup connections
       *
       * ConnectionBase is a helper class to ease the storage of Connections
       * which has specialized System information, into a uniform vector based
       * on pointers, in the Connections add-on in \elastica
       *
       * \tparam ConnectionRegistrars Registrars for all connections
       *
       * \see Connection
       */
      template <typename ConnectionRegistrars>
      class ConnectionBase : public NonCopyable {
       protected:
        //**Type definitions****************************************************
        //! Type of the final connection applicator
        using ConnecterID = std::unique_ptr<ConnecterBase>;
        //! Type of the Unique ID
        using UUIDType = ConnectionUUID;
        //! Type of the index
        using IndexType = typename UUIDType::IndexType;
        //**********************************************************************

        // Friend declaration with the main Connections class (for finalize)
        template <class SimTraits,  // Simulator options
                  class Sim>        // Simulator class
        friend class Connections;

        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The sole constructor.
         *
         * \param id An integer ID of the current connection
         * \param name Name of the current Connection
         */
        ConnectionBase(IndexType id, std::string&& name)
            : connection_id_(id, std::move(name)),
              concrete_connection_type_(),
              first_concrete_system_type_(),
              second_concrete_system_type_(),
              connecter_generator_(nullptr){};
        //**********************************************************************

        //@}
        //**********************************************************************

       public:
        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        //**********************************************************************
        /*!\brief Destructor
         */
        virtual ~ConnectionBase() = default;
        //**********************************************************************
        //@}
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief The finalize method.
         *
         * \details
         * The finalize method is the main interface to get the finalized
         * Connecter that will act on systems within \elastica
         */
        virtual ConnecterID finalize() const = 0;
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Get ID
         *
         * \return UUID
         *
         * \note
         * Returns by value, doesn't usually matter as its cheap with small
         * string optimization
         */
        UUIDType id() const { return connection_id_; }
        //**********************************************************************

       protected:
        //**********************************************************************
        /*!\brief Sets the name of the connection
         *
         * \param name of the connection
         * \return void
         */
        inline constexpr auto set_name() & noexcept -> std::string&{
          /*
           * Dev note:
           * 1. noexcept because of copy-and-swap
           * 2. This takes in an argument because this is exposed as an
           * interface to the connection class
           */
          return connection_id_.name;
        }
        //**********************************************************************

       protected:
        //**********************************************************************
        /*!\brief Set pretty name of the first type of system being connected
         *
         * \details
         * Explicitly have a "set" in the interface but thats okay since its for
         * internal use only
         *
         * \return Mutable reference to std::string
         */
        inline constexpr auto set_first_concrete_system_type() & noexcept
            -> std::string& {
          return first_concrete_system_type_;
        }
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Get pretty name of the first type of system being connected
         *
         * \return Const reference to std::string
         */
        inline constexpr auto first_concrete_system_type() const& noexcept
            -> std::string const& {
          return first_concrete_system_type_;
        }
        //**********************************************************************

       protected:
        //**********************************************************************
        /*!\brief Set pretty name of the second type of system being connected
         *
         * \details
         * Explicitly have a "set" in the interface but thats okay since its for
         * internal use only
         *
         * \return Mutable reference to std::string
         */
        inline constexpr auto set_second_concrete_system_type() & noexcept
            -> std::string& {
          return second_concrete_system_type_;
        }
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Get pretty name of the second type of system being connected
         *
         * \return Const reference to std::string
         */
        inline constexpr auto second_concrete_system_type() const& noexcept
            -> std::string const& {
          return second_concrete_system_type_;
        }
        //**********************************************************************

       protected:
        //**********************************************************************
        /*!\brief Set pretty name of the type of connection being applied to the
         * system
         *
         * \details
         * Explicitly have a "set" in the interface but thats okay since its for
         * internal use only
         *
         * \return std::string
         */
        inline constexpr auto set_concrete_connection_type() & noexcept
            -> std::string& {
          return concrete_connection_type_;
        }
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Get pretty name of the type of connection being applied to the
         * system
         *
         * \return Const reference to std::string
         */
        inline constexpr auto concrete_connection_type() const& noexcept
            -> std::string const& {
          return concrete_connection_type_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Check if the current Connection has all necessary information
         * for making a connecter
         *
         * \return bool
         */
        virtual bool is_not_valid() const noexcept = 0;
        //**********************************************************************

       protected:
        //**Member variables****************************************************
        //! A unique id for each connection, for debugging purposes
        UUIDType connection_id_;
        //! Pretty name of the type of connection
        std::string concrete_connection_type_;
        //! Pretty name of the type of first system being connected
        std::string first_concrete_system_type_;
        //! Pretty name of the type of second system being connected
        std::string second_concrete_system_type_;
        //! Generator for the final connecter object
        std::function<ConnecterID()> connecter_generator_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace connections

  }  // namespace interface

}  // namespace elastica
