//==============================================================================
/*!
//  \file
//  \brief Forcer base class
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
///
#include "Simulator/Interface/Forcing/Types.hpp"
///
#include "Simulator/Interface/Forcing/ForcingUUID.hpp"
/// Serialization support
#include "Simulator/IO/Serialization/Types.hpp"
/// Time
#include "Time/SimulationTime.hpp"
///
#include "Utilities/NonCopyable.hpp"

namespace elastica {

  namespace interface {

    namespace forcing {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Base class for a Forcing applicator object
       * \ingroup forcing
       *
       * ForcerBase is a helper class to implement the storage of a
       * forcing, the system it acts on and the indices/range of indices it
       * needs to act on, within \elastica
       *
       * \see Forcer, Forcing
       */
      class ForcerBase : NonCopyable {
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
         * \param id ID of forcing, passed into the constrainer
         */
        explicit ForcerBase(ForcingUUID id) : id_(std::move(id)){};
        //**********************************************************************

        //**********************************************************************
        /*!\brief The move constructor.
         */
        ForcerBase(ForcerBase&& other) noexcept : id_(std::move(other.id_)){};
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
        virtual ~ForcerBase() = default;
        //@}
        //**********************************************************************

        //**Interfaces**********************************************************
        /*!\name Interface methods */
        //@{

        //**********************************************************************
        /*!\brief Force translational degrees of freedom of a system
         *
         * \param time_v The current time of simulation
         */
        virtual void apply_forces(TimeType /* time_v */) const noexcept = 0;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Force rotational degrees of freedom  of a system
         *
         * \param time_v The current time of simulation
         */
        virtual void apply_torques(TimeType /* time_v */) const noexcept = 0;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets a handle to the stored forcing with its type erased
         */
        virtual void* get_forcing_unsafe() noexcept = 0;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Force both translational and rotation DOFs simultaneously
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

       public:
        //**Friendships*********************************************************
        //! Friendship with Serialize
        friend struct ::elastica::io::Serialize<ForcerBase>;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Get ID
         *
         * \return reference to UUID
         */
        auto id() & -> ForcingUUID& { return id_; }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Get ID
         *
         * \return UUID
         */
        auto id() const& -> ForcingUUID { return id_; }
        //**********************************************************************

       protected:
        //**Member variables****************************************************
        //! A unique id for each forcing, for debugging purposes
        ForcingUUID id_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace forcing

  }  // namespace interface

}  // namespace elastica
