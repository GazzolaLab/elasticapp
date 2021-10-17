//==============================================================================
/*!
//  \file
//  \brief Constrainer base class
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

#include <utility>
/// Constraints
#include "Simulator/Interface/Constraints/Types.hpp"
//
#include "Simulator/Interface/Constraints/ConstraintUUID.hpp"
/// Serialization support
#include "Simulator/IO/Serialization/Types.hpp"
/// Time
#include "Time/SimulationTime.hpp"
///
#include "Utilities/NonCopyable.hpp"

namespace elastica {

  namespace interface {

    namespace constraints {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Base class for a Constraint applicator object
       * \ingroup constraints
       *
       * ConstrainerBase is a helper class to implement the storage of a
       * constraint, the system it acts on and the indices/range of indices it
       * needs to act on, within \elastica
       *
       * \see Constrainer, Constraint
       */
      class ConstrainerBase : NonCopyable {
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
         * \param id ID of constraint, passed into the constrainer
        */
        explicit ConstrainerBase(ConstraintUUID id) : id_(std::move(id)){};
        //**********************************************************************

        //**********************************************************************
        /*!\brief The move constructor.
         */
        ConstrainerBase(ConstrainerBase&& other) noexcept
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
        virtual ~ConstrainerBase() = default;
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Interfaces**********************************************************
        /*!\name Interface methods */
        //@{

        //**********************************************************************
        /*!\brief Constrain primitives (positions, directors) of a system
         *
         * \param time_v The current time of simulation
         */
        virtual void constrain_primitives(
            TimeType /* time_v */) const noexcept = 0;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Constrain rates (velocities, angular velocity) of a system
         *
         * \param time_v The current time of simulation
         */
        virtual void constrain_rates(TimeType /* time_v */) const noexcept = 0;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets a handle to the stored constraint its type erased
         */
        virtual void* get_constraint_unsafe() noexcept = 0;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Constrain both primitives and rates simultaneously
         *
         * \param time_v The current time of simulation
         *
         * \note
         * is this a good idea through?
         */
        void operator()(TimeType time_v) const noexcept {
          constrain_primitives(time_v);
          constrain_rates(time_v);
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       public:
        //**Friendships*********************************************************
        //! Friendship with Serialize
        friend struct ::elastica::io::Serialize<ConstrainerBase>;
        //**********************************************************************

        //**********************************************************************
        /*!\brief Get ID
         *
         * \return reference to UUID
         */
        auto id() & -> ConstraintUUID& { return id_; }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Get ID
         *
         * \return UUID
         */
        auto id() const& -> ConstraintUUID { return id_; }
        //**********************************************************************

       protected:
        //**Member variables****************************************************
        //! A unique id for each constraint, for debugging purposes
        ConstraintUUID id_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace constraints

  }  // namespace interface

}  // namespace elastica