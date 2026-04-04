//==============================================================================
/*!
//  \file
//  \brief Connecter base class
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

#include <utility>
/// Connections
#include "Simulator/Interface/Connections/Types.hpp"
//
#include "Simulator/Interface/Connections/ConnectionUUID.hpp"
/// Serialization
#include "Simulator/IO/Serialization/Types.hpp"
/// Time
#include "Time/SimulationTime.hpp"
/// Utilities
#include "Utilities/NonCopyable.hpp"

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
      /*!\brief Base class for a Connection applicator object
       * \ingroup connections
       *
       * ConnecterBase is a helper class to implement the storage of a
       * connection, the system(s) and the indices it needs to act on,
       * within \elastica
       *
       * \see Connecter, Connection
       */
      class ConnecterBase : NonCopyable {
       protected:
        //**Type definitions****************************************************
        //! Type of the time
        using TimeType = Time;
        //**********************************************************************

        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The default constructor.
         *
         * \param id ID of connection, passed into the connecter
        */
        explicit ConnecterBase(ConnectionUUID id) : id_(std::move(id)){};
        //**********************************************************************

        //**********************************************************************
        /*!\brief The move constructor.
         */
        ConnecterBase(ConnecterBase&& other) noexcept
            : id_(std::move(other.id_)){};
        //**********************************************************************

        //@}
        //**********************************************************************

       public:
        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        //**********************************************************************
        /*!\brief Virtual destructor
         */
        virtual ~ConnecterBase() = default;
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Interfaces**********************************************************
        /*!\name Interface methods */
        //@{

        //**********************************************************************
        /*!\brief Apply forces due to a connection between systems
         *
         * \param time_v The current time of simulation
        */
        virtual void apply_forces(TimeType /* time_v */) const noexcept = 0;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply torques due to a connection between systems
         *
         * \param time_v The current time of simulation
         */
        virtual void apply_torques(TimeType /* time_v */) const noexcept = 0;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets a handle to the stored connection its type erased
         */
        virtual void* get_connection_unsafe() noexcept = 0;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply all connection artefacts simultaneously
         *
         * \param time_v The current time of simulation
         *
         * \note
         * is this a good idea through?
         */
        void operator()(TimeType time_v) const noexcept {
          apply_forces(time_v);
          apply_torques(time_v);
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**Friendships*********************************************************
        //! Friendship with Serialize
        friend struct ::elastica::io::Serialize<ConnecterBase>;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Get ID
         *
         * \return reference to UUID
         */
        auto id() & -> ConnectionUUID& { return id_; }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Get ID
         *
         * \return UUID
         */
        auto id() const& -> ConnectionUUID { return id_; }
        //**********************************************************************

       protected:
        //**Member variables****************************************************
        //! A unique id for each connection, for debugging purposes
        ConnectionUUID id_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace connections

  }  // namespace interface

}  // namespace elastica
