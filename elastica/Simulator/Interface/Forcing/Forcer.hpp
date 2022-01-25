//==============================================================================
/*!
//  \file
//  \brief Forcer class to apply forcing during runtime
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
#include <cstdint>
#include <memory>

///
#include "Simulator/Forcing/Forcing.hpp"
///
#include "Simulator/Interface/Forcing/ForcerBase.hpp"
#include "Simulator/Interface/Forcing/ForcingUUID.hpp"
#include "Simulator/Interface/Forcing/Types.hpp"
///
#include "Utilities/NumericAt.hpp"
#include "Utilities/NumericRange.hpp"

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
      /*!\brief A Forcing applicator object, specialized for forcing
       * whose information is fully known at compile-time
       * \ingroup forcing
       *
       * \details
       * Forcer enforces a forcing in \elastica on a system over
       * a range of indices.
       *
       * \tparam System system being forced
       * \tparam ConcreteForcing forcing being applied
       * \tparam IndexSet a set of indices of the system on which we apply the
       * forcing, can be a compact set or a range
       *
       * \note
       * This specialization unrolls interior loops in case we have a force at
       * more than one index
       *
       * \see Forcing
       */
      template <typename System, typename ConcreteForcing, typename IndexSet>
      class Forcer /*final*/ : public ForcerBase {
       public:
        //**Type definitions****************************************************
        //! Type of the system
        using SystemType = System;
        //! Type of the forcing
        using ForcingType = ConcreteForcing;
        //! Type of the index set
        using IndexType = IndexSet;
        //! Type of the time
        using ForcerBase::TimeType;
        //**********************************************************************

       public:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The default constructor.
         *
         * \param uuid UUID of forcing
         * \param sys system
         * \param c Forcing
         * \param index_set indexset
         */
        Forcer(ForcingUUID uuid, SystemType const& sys, ForcingType c,
               IndexType index_set)
            : ForcerBase(std::move(uuid)),
              sys_(sys),
              forcing_(std::move(c)),
              indices_(std::move(index_set)){};
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        //**********************************************************************
        /*!\brief destructor
         */
        ~Forcer() override = default;
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
        /*!\brief Force translational degrees of freedom of a system
         *
         * \param time_v The current time of simulation
         */
        void apply_forces(TimeType time_v) const noexcept final {
          forcing_.apply_forces_internal(time_v, sys_, indices_);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Force rotational degrees of freedom  of a system
         *
         * \param time_v The current time of simulation
         */
        void apply_torques(TimeType time_v) const noexcept final {
          forcing_.apply_torques_internal(time_v, sys_, indices_);
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets a handle to the stored forcing with its type erased
         */
        void* get_forcing_unsafe() noexcept final { return &forcing_; }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! Reference to system being forced
        mutable SystemType sys_;
        // Wow<sizeof(int&)> wow;
        //! Forcing object
        ForcingType forcing_;
        //! Index set to apply forcing on
        IndexType indices_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief A Forcing applicator object, specialized for runtime
       * forcing
       * \ingroup forcing
       *
       * \details
       * Forcer enforces a forcing in \elastica on a system over
       * a range of indices.
       *
       * \tparam System system being forced
       * \tparam ForcingRegistrars Registered forcing
       * \tparam BaseOfIndexSet Base class of index set (which is either a
       * compact set or a range)
       *
       * \note
       * This is a specialization for runtime forcing, and so has two
       * additional indirections
       *
       * \see Forcing
       */
      template <typename System, typename ForcingRegistrars,
                typename BaseOfIndexSet>
      class Forcer<System,
                   std::unique_ptr<::elastica::forcing::detail::ForcingBase<
                       ForcingRegistrars>>,
                   std::unique_ptr<BaseOfIndexSet>>
          /*final*/ : public ForcerBase {
       public:
        //**Type definitions****************************************************
        //! Type of the system
        using SystemType = System;
        //! Type of the forcing
        using ForcingType = std::unique_ptr<
            ::elastica::forcing::detail::ForcingBase<ForcingRegistrars>>;
        //! Type of the index set
        using IndexType = std::unique_ptr<BaseOfIndexSet>;
        //! Type of the time
        using ForcerBase::TimeType;
        //**********************************************************************

       public:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The default constructor.
         *
         * \param uuid UUID of forcing
         * \param sys system
         * \param c Forcing
         * \param index_set indexset
         */
        Forcer(ForcingUUID uuid, SystemType const& sys, ForcingType c,
               IndexType index_set)
            : ForcerBase(std::move(uuid)),
              sys_(sys),
              forcing_(std::move(c)),
              indices_(std::move(index_set)){};
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        //**********************************************************************
        /*!\brief destructor
         */
        ~Forcer() override = default;
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
        virtual void apply_forces(TimeType time_v) const noexcept final {
          apply_forces_implementation(time_v);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Force rotational degrees of freedom  of a system
         *
         * \param time_v The current time of simulation
         */
        virtual void apply_torques(TimeType time_v) const noexcept final {
          apply_torques_implementation(time_v);
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets a handle to the stored forcing with its type erased
         */
        void* get_forcing_unsafe() noexcept final { return forcing_.get(); }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**********************************************************************
        /*!\brief Force translational degrees of freedom of a system for a range
         *
         * \param time_v The current time of simulation
         *
         * \note
         * SFINAE rather than tag dispatch as apply_forces_internal requires
         * only one compilation path
         */
        template <typename X = BaseOfIndexSet,
                  Requires<::elastica::tt::IsNumericRange<X>::value> = nullptr>
        inline void apply_forces_implementation(
            TimeType time_v) const noexcept {
          forcing_->apply_forces_internal(time_v, sys_, *indices_);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Force translational degrees of freedom of a system for a set
         *
         * \param time_v The current time of simulation
         *
         * \note
         * SFINAE rather than tag dispatch as apply_forces_internal requires
         * only one compilation path
         */
        template <
            typename X = BaseOfIndexSet,
            Requires<not ::elastica::tt::IsNumericRange<X>::value> = nullptr>
        inline void apply_forces_implementation(
            TimeType time_v) const noexcept {
          forcing_->apply_forces_internal(time_v, sys_, indices_->data(),
                                          indices_->size());
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Force translational degrees of freedom of a system for a range
         *
         * \param time_v The current time of simulation
         *
         * \note
         * SFINAE rather than tag dispatch as apply_torques_internal requires
         * only one compilation path
         */
        template <typename X = BaseOfIndexSet,
                  Requires<::elastica::tt::IsNumericRange<X>::value> = nullptr>
        inline void apply_torques_implementation(
            TimeType time_v) const noexcept {
          forcing_->apply_torques_internal(time_v, sys_, *indices_);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Force translational degrees of freedom of a system for a set
         *
         * \param time_v The current time of simulation
         *
         * \note
         * SFINAE rather than tag dispatch as apply_torques_internal requires
         * only one compilation path
         */
        template <
            typename X = BaseOfIndexSet,
            Requires<not ::elastica::tt::IsNumericRange<X>::value> = nullptr>
        inline void apply_torques_implementation(
            TimeType time_v) const noexcept {
          forcing_->apply_torques_internal(time_v, sys_, indices_->data(),
                                           indices_->size());
        }
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! Reference to system being forced
        mutable SystemType sys_;
        //! Pointer to a runtime forcing
        ForcingType forcing_;
        //! Pointer to a set of runtime indices
        IndexType indices_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Make an \elastica forcer in an idiomatic way
       * \ingroup forcing
       *
       * \details
       * We use make_forcer() to get a Forcer object which can then be stored
       * to apply forcing during runtime
       *
       * \note
       * There is no type checking for the index set here and it takes any type
       * meeting the index set concept
       *
       * \see Forcer
       */
      template <typename System, typename ConcreteForcing,
                typename IndexSetOrRange>
      inline auto make_forcer(
          ForcingUUID id, System const& sys, ConcreteForcing&& bc,
          IndexSetOrRange&& index_set_or_range) /* noexcept */
          -> std::unique_ptr<ForcerBase> {
        return std::unique_ptr<ForcerBase>(
            new Forcer<System, std::decay_t<ConcreteForcing>,
                       std::decay_t<IndexSetOrRange>>(
                std::move(id), sys, std::forward<ConcreteForcing>(bc),
                std::forward<IndexSetOrRange>(index_set_or_range)));
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Make an \elastica forcer in an idiomatic way
       * \ingroup forcing
       *
       * \details
       * We use make_forcer() to get a Forcer object which can then be stored
       * to apply forcing during runtime
       *
       * \note
       * This is a specialization for numeric_at_t
       *
       * \see Forcer
       */
      template <typename System, typename ForcingRegistrars>
      inline auto make_forcer(
          ForcingUUID id, System const& sys,
          std::unique_ptr<
              ::elastica::forcing::detail::ForcingBase<ForcingRegistrars>>
              bc_ptr,
          std::unique_ptr<::elastica::detail::numeric_at_base_t> at_ptr)
          -> std::unique_ptr<ForcerBase> {
        return std::unique_ptr<ForcerBase>(
            new Forcer<System,
                       std::unique_ptr<::elastica::forcing::detail::ForcingBase<
                           ForcingRegistrars>>,
                       std::unique_ptr<::elastica::detail::numeric_at_base_t>>(
                std::move(id), sys, std::move(bc_ptr), std::move(at_ptr)));
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Make an \elastica forcer in an idiomatic way
       * \ingroup forcing
       *
       * \details
       * We use make_forcer() to get a Forcer object which can then be stored
       * to apply forcing during runtime
       *
       * \see Forcer
       */
      template <typename System, typename ForcingRegistrars>
      inline auto make_forcer(
          ForcingUUID id, System const& sys,
          std::unique_ptr<
              ::elastica::forcing::detail::ForcingBase<ForcingRegistrars>>
              bc_ptr,
          std::unique_ptr<::elastica::detail::NumericRange> at_ptr)
          -> std::unique_ptr<ForcerBase> {
        return std::unique_ptr<ForcerBase>(
            new Forcer<System,
                       std::unique_ptr<::elastica::forcing::detail::ForcingBase<
                           ForcingRegistrars>>,
                       std::unique_ptr<::elastica::detail::NumericRange>>(
                std::move(id), sys, std::move(bc_ptr), std::move(at_ptr)));
      }
      /*! \endcond */
      //************************************************************************

    }  // namespace forcing

  }  // namespace interface

}  // namespace elastica
