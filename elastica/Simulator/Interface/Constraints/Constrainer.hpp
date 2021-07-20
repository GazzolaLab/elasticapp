//==============================================================================
/*!
//  \file
//  \brief Constrainer class to apply constraints during runtime
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

#include <array>
#include <cstddef>  // size_t
#include <memory>

///
#include "Simulator/Constraints/Constraint.hpp"
#include "Simulator/Constraints/InitialConditionsCache.hpp"
///
#include "Simulator/Interface/Constraints/ConstrainerBase.hpp"
#include "Simulator/Interface/Constraints/ConstraintUUID.hpp"
#include "Simulator/Interface/Constraints/Types.hpp"
///
#include "Utilities/NumericAt.hpp"

/*
 * Developer notes:
 * The design space here is somewhat orthogonal (time vs space) and so a choice
 * was made
 *
 * There are two main choices here:
 * 1) Have a base class for constrainer, and then have a constrainer class
 * which can have variable templates, like so:
 * struct Base{}
 * template <typename Sys, std::size_t N>
 * struct Constrainer : Base{
 *  std::array<size_t, N> indices;
 * }
 * then we can do in Constrainer's get rates method
 * bc->constrain_rates(sys, idx[0], idx[1], ... idx[N])
 * which is advantageous to pass as a variadic list (for reasons mentioned
 bwlow)
 *
 * 2) We dont have a base class and instead have a Constrainer class :
 * template <typename Sys>
 * struct Constrainer{
 *
 *  std::vector indices (can be array too with a max size)
 * };
 * And then in interface::Constraints, we have a vector of Constrainer<Sys> for
 * all possible system types (similar to the main Simulator). we then directly
 * apply constrain methods here like so:
 *
 * for idx : possible_indices(
 *  bc->constrain_rates(sys, idx)
   );
 *
 * The advantage of the first approach are:
 * - it needs only a vector in the interface::Constraints class
 * - The variadic list reduces the call_with_dynamic_type calls in BC, because
 * the type only needs to be figured out once, as opposed to 2) where you
 *  essentially call the constrain rates in a loop
 *
 * The advantages of the second approach are:
 * - reduce one virtual call without Base
 *
 * I expect 1) to be faster when there are multiple constrain indices (quite
 * possible for our rods) while 2) will be faster for small number of systems
 * with single index constrain. Also the cost of looping over all possible
 * systems may make 1) slightly better.
 */

// TODO : Have an array data-structure with copy of initial positions,
// directors etc.
// TODO : Meta-porgram based on the "initial_conditions" argument in the
// derived class---have a policy class for when initial conditions are needed
// and when they are not

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
      /*!\brief A Constraint applicator object, specialized for constraints
       * whose information is fully known at compile-time
       * \ingroup constraints
       *
       * \details
       * Constrainer enforces a constraint in \elastica on a system over
       * a range of indices.
       *
       * \tparam System system being constrained
       * \tparam ConcreteConstraint constraint being applied
       * \tparam IndexSet a set of indices of the system on which we apply the
       * constraint
       *
       * \note
       * This is a specialization that unrolls interior loops
       *
       * \see Constraint
       */
      template <typename System, typename ConcreteConstraint, typename IndexSet>
      class Constrainer /*final*/ : public ConstrainerBase {
        /*
         * Dev note:
         * Not marked final because testing needs derivation
         */
       public:
        //**Type definitions****************************************************
        //! Type of the system
        using SystemType = System;
        //! Type of the constraint
        using ConstraintType = ConcreteConstraint;
        //! Type of the index set
        using IndexType = IndexSet;
        //! Type of the time
        using ConstrainerBase::TimeType;
        //**********************************************************************

       public:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The default constructor.
         *
         * \param uuid UUID of constraint
         * \param sys system
         * \param c Constraint
         * \param index_set indexset
         */
        Constrainer(ConstraintUUID uuid, SystemType& sys, ConstraintType&& c,
                    IndexType&& index_set)
            : ConstrainerBase(std::move(uuid)),
              indices_(std::move(index_set)),
              sys_(sys),
              constraint_(std::move(c)){};
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        //**********************************************************************
        /*!\brief destructor
         */
        ~Constrainer() override = default;
        //**********************************************************************
        //@}
        //**********************************************************************

       protected:
        //**Utility*************************************************************
        /*!\name Utility methods */
        //@{

        //**********************************************************************
        /*!\brief Gets the index buffer
         */
        inline constexpr auto indices() & noexcept -> IndexType& {
          return indices_;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets the index buffer
         */
        inline constexpr auto indices() const& noexcept -> IndexType const& {
          return indices_;
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       public:
        //**Interfaces**********************************************************
        /*!\name Interface methods */
        //@{

        //**********************************************************************
        /*!\brief Constrain primitives (positions, directors) of a system
         *
         * \param time_v The current time of simulation
         */
        void constrain_primitives(TimeType time_v) const noexcept final {
          constraint_.constrain_primitives_internal(time_v, sys_, indices());
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Constrain rates (velocities, angular velocity) of a system
         *
         * \param time_v The current time of simulation
         */
        void constrain_rates(TimeType time_v) const noexcept final {
          constraint_.constrain_rates_internal(time_v, sys_, indices());
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets a handle to the stored constraint its type erased
         */
        void* get_constraint_unsafe() noexcept final {
          return &constraint_;
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! Index set to apply constraint on
        IndexType indices_;
        //! Reference to system being constrained
        SystemType& sys_;
        //! Constraint object
        ConstraintType constraint_;
        // //! Initial conditions
        // InitialConditions ics_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief A Constraint applicator object, specialized for runtime
       * constraints
       * \ingroup constraints
       *
       * \details
       * Constrainer enforces a constraint in \elastica on a system over
       * a range of indices.
       *
       * \tparam System system being constrained
       * \tparam ConstraintRegistrars Registered constraints
       *
       * \note
       * This is a specialization for runtime constraints, and so has two
       * additional indirections
       *
       * \see Constraint
       */
      template <typename System, typename ConstraintRegistrars>
      class Constrainer<
          System,
          std::unique_ptr<::elastica::constraints::detail::ConstraintBase<
              ConstraintRegistrars>>,
          std::unique_ptr<::elastica::detail::numeric_at_base_t>>
          /*final*/ : public ConstrainerBase {
        /*
         * Dev note:
         * Not marked final because testing needs derivation
         */
       public:
        //**Type definitions****************************************************
        //! Type of the system
        using SystemType = System;
        //! Type of the constraint
        using ConstraintType =
            std::unique_ptr<::elastica::constraints::detail::ConstraintBase<
                ConstraintRegistrars>>;
        //! Type of the index set
        using IndexType =
            std::unique_ptr<::elastica::detail::numeric_at_base_t>;
        //! Type of the time
        using ConstrainerBase::TimeType;
        //**********************************************************************

       public:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The default constructor.
         *
         * \param uuid UUID of constraint
         * \param sys system
         * \param c Constraint
         * \param index_set indexset
         */
        Constrainer(ConstraintUUID uuid, SystemType& sys, ConstraintType c,
                    IndexType index_set)
            : ConstrainerBase(std::move(uuid)),
              indices_(std::move(index_set)),
              sys_(sys),
              constraint_(std::move(c)){};
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        //**********************************************************************
        /*!\brief destructor
         */
        ~Constrainer() override = default;
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
        void constrain_primitives(TimeType time_v) const noexcept final {
          // unsafe ptr?
          constraint_->constrain_primitives_internal(
              time_v, sys_, indices_->data(), indices_->size());
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Constrain rates (velocities, angular velocity) of a system
         *
         * \param time_v The current time of simulation
         */
        void constrain_rates(TimeType time_v) const noexcept final {
          constraint_->constrain_rates_internal(time_v, sys_, indices_->data(),
                                                indices_->size());
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets a handle to the stored constraint its type erased
         */
        void* get_constraint_unsafe() noexcept final {
          return constraint_.get();
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! Pointer to a set of runtime indices
        IndexType indices_;
        //! Reference to system being constrained
        SystemType& sys_;
        //! Pointer to a runtime constraint
        ConstraintType constraint_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Make an \elastica constrainer in an idiomatic way
       * \ingroup constraints
       *
       * \details
       * We use make_constrainer() to get a Constrainer object
       * which can then be stored to apply constraint during runtime
       *
       * \see Constrainer
       */
      template <typename System, typename ConcreteConstraint, std::uint8_t N>
      inline auto make_constrainer(
          const ConstraintUUID& id, System& sys, ConcreteConstraint&& bc,
          ::elastica::detail::numeric_at_t<N>&& at) /* noexcept */
          -> std::unique_ptr<ConstrainerBase> {
        return std::unique_ptr<ConstrainerBase>(
            new Constrainer<System, std::decay_t<ConcreteConstraint>,
                            ::elastica::detail::numeric_at_t<N>>(
                id, sys, std::forward<ConcreteConstraint>(bc), std::move(at)));
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Make an \elastica constrainer in an idiomatic way
       * \ingroup constraints
       *
       * \details
       * We use make_constrainer() to get a Constrainer object
       * which can then be stored to apply constraint during runtime
       *
       * \see Constrainer
       */
      template <typename System, typename ConstraintRegistrars>
      inline auto make_constrainer(
          const ConstraintUUID& id, System& sys,
          std::unique_ptr<::elastica::constraints::detail::ConstraintBase<
              ConstraintRegistrars>>
              bc_ptr,
          std::unique_ptr<::elastica::detail::numeric_at_base_t> at_ptr)
          -> std::unique_ptr<ConstrainerBase> {
        return std::unique_ptr<ConstrainerBase>(
            new Constrainer<
                System,
                std::unique_ptr<::elastica::constraints::detail::ConstraintBase<
                    ConstraintRegistrars>>,
                std::unique_ptr<::elastica::detail::numeric_at_base_t>>(
                id, sys, std::move(bc_ptr), std::move(at_ptr)));
      }
      /*! \endcond */
      //************************************************************************

    }  // namespace constraints

  }  // namespace interface

}  // namespace elastica
