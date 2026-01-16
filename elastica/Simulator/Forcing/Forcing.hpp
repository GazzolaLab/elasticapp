//==============================================================================
/*!
//  \file
//  \brief Base class for all forcing
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

#include <type_traits>

///
#include "Simulator/Forcing/Types.hpp"
///
#include "Simulator/Forcing/Protocols.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/FakeVirtual.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/NoSuchType.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/NumericAt.hpp"
#include "Utilities/NumericRange.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/Registration.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace elastica {

  namespace forcing {

    namespace detail {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Abstract class for storing forcings within \elastica
       * \ingroup forcing
       *
       * \details
       * ForcingBase is an abstract class to access forcings during
       * runtime in \elastica.
       *
       * \tparam Registrars Registration parameter
       */
      template <typename Registrars>
      class ForcingBase
          : NonCopyable,
            public ::tt::ConformsTo<::elastica::forcing::protocols::Forcing> {
       public:
        //**Type definitions****************************************************
       private:
        //! All creatable (and callable) classes
        using creatable_classes = Registration::registrants<Registrars>;
        //**********************************************************************

       protected:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The default constructor.
         */
        ForcingBase() noexcept = default;
        //**********************************************************************

        // explicitly declare the move constructors for safety

        //**********************************************************************
        /*!\brief The move constructor.
         */
        ForcingBase(ForcingBase&&) noexcept = default;
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Assignment Operators************************************************
        /*!\name Assignment Operators */
        //@{

        //**********************************************************************
        /*!\brief The move assignment operator.
         */
        ForcingBase& operator=(ForcingBase&&) noexcept = default;
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
        virtual ~ForcingBase() = default;
        //@}
        //**********************************************************************

        //**Forcing functions************************************************
        /*!\name Forcing functions */
        //@{

        //**********************************************************************
        /*!\brief Apply forces
         *
         * \details
         * Overloaded compile-time variant for a couple of indices
         */
        template <typename TimeType, typename System, std::uint8_t N>
        auto apply_forces_internal(
            TimeType t, System& sys,
            ::elastica::detail::numeric_at_t<N> const& index_set) const
            -> void {
          return call_with_dynamic_type<void, creatable_classes>(
              this, [&, t = std::move(t)](auto* const forcing) {
                return forcing->apply_forces_internal(t, sys, index_set);
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply forces
         *
         * \details
         * Overloaded runtime variant for a couple of indices
         */
        template <typename TimeType, typename System>
        auto apply_forces_internal(TimeType t, System& sys,
                                   const std::size_t* const index_data,
                                   std::size_t n_indices) const -> void {
          return call_with_dynamic_type<void, creatable_classes>(
              this, [&, t = std::move(t)](auto* const forcing) {
                return forcing->apply_forces_internal(t, sys, index_data,
                                                      n_indices);
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply forces
         *
         * \details
         * Overloaded runtime variant for a range of indices
         */
        template <typename TimeType, typename System>
        auto apply_forces_internal(
            TimeType t, System& sys,
            ::elastica::detail::NumericRange const& range) const -> void {
          return call_with_dynamic_type<void, creatable_classes>(
              this, [&, t = std::move(t)](auto* const forcing) {
                return forcing->apply_forces_internal(t, sys, range);
              });
        }
        //**********************************************************************

        //@}
        //**********************************************************************

        /*!\name Torque apply functions */
        //@{

        //**********************************************************************
        /*!\brief Apply torques
         *
         * \details
         * Overloaded compile-time variant for a couple of indices
         */
        template <typename TimeType, typename System, std::uint8_t N>
        auto apply_torques_internal(
            TimeType t, System& sys,
            ::elastica::detail::numeric_at_t<N> const& index_set) const
            -> void {
          return call_with_dynamic_type<void, creatable_classes>(
              this, [&, t = std::move(t)](auto* const forcing) {
                return forcing->apply_torques_internal(t, sys, index_set);
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply torques
         *
         * \details
         * Overloaded runtime variant for a couple of indices
         */
        template <typename TimeType, typename System>
        auto apply_torques_internal(TimeType t, System& sys,
                                    const std::size_t* const index_data,
                                    std::size_t n_indices) const -> void {
          return call_with_dynamic_type<void, creatable_classes>(
              this, [&, t = std::move(t)](auto* const forcing) {
                return forcing->apply_torques_internal(t, sys, index_data,
                                                       n_indices);
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply torques
         *
         * \details
         * Overloaded runtime variant for a range of indices
         */
        template <typename TimeType, typename System>
        auto apply_torques_internal(
            TimeType t, System& sys,
            ::elastica::detail::NumericRange const& range) const -> void {
          return call_with_dynamic_type<void, creatable_classes>(
              this, [&, t = std::move(t)](auto* const forcing) {
                return forcing->apply_torques_internal(t, sys, range);
              });
        }
        //**********************************************************************
        //@}
        //************************************************************************

        //@}
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Signature of expected forcing
       * \ingroup forcing
       *
       * \note
       * We have the function parameters as class parameters here, and defer the
       * actual type to be a type-alias parameter because the other way around
       * is not possible. Because instantiating a Signatures<Forcing> checks for
       * a apply_forces_on_range() method immediately on Forcing (since its
       * known at class instantiation time), which may not be available for all
       * forcings. Doing it the other way, we wait till function instantiation
       * time where more context is available.
       *
       * \tparam TimeType Type of time
       * \tparam System Type of system
       */
      template <typename TimeType, typename System>
      struct Signatures {
        /// [apply_forces_signature]
        template <typename Forcing>
        using apply_forces_signature =
            decltype(std::declval<Forcing const&>().apply_forces(
                std::declval<TimeType>(), std::declval<System&>(),
                std::declval<std::size_t>()));
        /* // This is equivalent to
         * template <typename TimeType, typename System>
         * void apply_forces(TimeType t, System& sys, std::size_t index)
         * const {
         *   // do something
         * }
         */
        /// [apply_forces_signature]

        /// [apply_forces_on_range_signature]
        template <typename Forcing>
        using apply_forces_on_range_signature =
            decltype(std::declval<Forcing const&>().apply_forces_on_range(
                std::declval<TimeType>(), std::declval<System&>(),
                std::declval<std::size_t>(), std::declval<std::size_t>()));
        /* // This is equivalent to
         * template <typename TimeType, typename System>
         * void apply_forces_on_range(TimeType t, System& sys, std::size_t
         * start, std::size_t stop) const {
         *   // do something
         * }
         */
        /// [apply_forces_on_range_signature]

        /// [apply_torques_signature]
        template <typename Forcing>
        using apply_torques_signature =
            decltype(std::declval<Forcing const&>().apply_torques(
                std::declval<TimeType>(), std::declval<System&>(),
                std::declval<std::size_t>()));
        /* // This is equivalent to
         * template <typename TimeType, typename System>
         * void apply_torques(TimeType t, System& sys, std::size_t index)
         * const {
         *   // do something
         * }
         */
        /// [apply_torques_signature]

        /// [apply_torques_on_range_signature]
        template <typename Forcing>
        using apply_torques_on_range_signature =
            decltype(std::declval<Forcing const&>().apply_torques_on_range(
                std::declval<TimeType>(), std::declval<System&>(),
                std::declval<std::size_t>(), std::declval<std::size_t>()));
        /* // This is equivalent to
         * template <typename TimeType, typename System>
         * void apply_torques_on_range(TimeType t, System& sys, std::size_t
         * start, std::size_t stop) const {
         *   // do something
         * }
         */
        /// [apply_torques_on_range_signature]
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Warn user on finding no suitable methods
       * \ingroup constraints
       */
      template <typename Forceing, typename System>
      struct [[deprecated(R"warn(
The current Forcing (first template parameter) has no apply_force() method that
can act on the given System (second template parameter)
)warn")]] warn_on_finding_no_suitable_force_method{};
      template <typename Forceing, typename System>
      struct [[deprecated(R"warn(
The current Forcing (first template parameter) has no apply_torque() method that
can act on the given System (second template parameter)
)warn")]] warn_on_finding_no_suitable_torque_method{};
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Default implementation of forcing in \elastica
     * \ingroup forcing
     *
     * \details
     * Forcing implements the concept of an external force that dynamically
     * affects various degrees of freedom of an \elastica entity (such as rods,
     * rigid bodies etc.). Any valid forcing in \elastica needs to derive from
     * this class using the CRTP pattern, as shown in the example below (or
     * alternatively use the ELASTICA_DEFINE_FORCING() macro). A forcing can
     * choose to effect translational degrees of freedom only
     * (such as positions/velocities) using the apply_forces()
     * member function and/or rotational degrees of freedom (such as directors
     * and angular velocities) using the apply_torques() member function.
     * Optionally we can choose not to define one of the above functions
     * in which case we assume a no-operation.
     *
     * \usage
     * The expected prototype of these two methods are shown below:
     *
     * \snippet this apply_forces_signature
     *
     * where
     * - `t` is the current time of the simulation
     * - `sys` is an object modeling an \elastica system
     * - `index` is the index on which a force is applied.
     *
     * And the torque method has the following prototype:
     *
     * \snippet this apply_torques_signature
     *
     * where
     * - `t` is the current time of the simulation
     * - `sys` is an object modeling an \elastica system
     * - `index` is the index on which a torque is applied.
     *
     * We suggest the user to place all forcings within the
     * elastica::forcing namespace as shown in the examples below.
     * Additionally, we provide the helpful macros ELASTICA_DEFINE_FORCING()
     * and ELASTICA_REGISTER_FORCING() for reducing boiler-plate code. See their
     * documentations for more details.
     *
     * Finally, for convenience, we provide the
     * elastica::forcings::make_forcing() function that
     * eases creation of a Forcing in \elastica, and we suggest the user to
     * consult its documentation first.
     *
     * \example
     * A valid forcing with both functions specified is shown below.
     *
     * \snippet Simulator/Forcing/Test_Forcing.cpp fwam
     *
     * Alternatively you can chose to specify either one of them, such as
     * apply_forces()
     *
     * \snippet Simulator/Forcing/Test_Forcing.cpp fwfo
     *
     * or apply_torques()
     *
     * \snippet Simulator/Forcing/Test_Forcing.cpp fwto
     *
     * in which case a no-operation is assumed for the other function. It is
     * also possible to overload the functions for specific types, shown below
     * for apply_forces()
     *
     * \snippet Simulator/Forcing/Test_Forcing.cpp fwsf
     *
     * or even templated types
     *
     * \snippet Simulator/Forcing/Test_Forcing.cpp fwtf
     *
     * which proves useful for metaprogramming based on the type-category of a
     * \elastica system (such as a Cosserat rod or a rigid body etc.), because
     * systems in \elastica are implemented using static polymorphism following
     * the CRTP pattern below.
     *
     * \code
     *  template <typename TimeType, typename T>
     *  apply_forces(TimeType t, RigidBody<T>&, ...);
     *
     *  template <typename TimeType, typename T>
     *  apply_forces(TimeType t, RodType<T>&, ...);
     * \endcode
     *
     * Additionally, in case of distributed force/torque acting on a continuous
     * range of indices, it is possible to optimize it by explicit vectorized
     * instructions. For such cases, one can additionally use the `_on_range()`
     * methods whose prototypes are shown below.
     *
     * \snippet this apply_forces_on_range_signature
     *
     * where
     * - `t` is the current time of the simulation
     * - `sys` is an object modeling an \elastica system
     * - `start` is the starting index on which a distributed force is applied.
     * - `stop` is the stopping index on which a distributed force is applied.
     *
     * And the ranged torque method has the following prototype:
     *
     * \snippet this apply_torques_on_range_signature
     *
     * where
     * - `t` is the current time of the simulation
     * - `sys` is an object modeling an \elastica system
     * - `start` is the starting index on which a distributed force is applied.
     * - `stop` is the stopping index on which a distributed force is applied.
     *
     * and examples for ranged forces and torques are shown below.
     *
     * \snippet Simulator/Forcing/Test_Forcing.cpp fwamr
     *
     * As before, we can choose to provide only one of forces or torques method
     * \snippet Simulator/Forcing/Test_Forcing.cpp fwfor
     * \snippet Simulator/Forcing/Test_Forcing.cpp fwtor
     *
     * and overloading and template specialization works as before.
     *
     * \tparam Registrars Registrars of derived classes, see Registration
     * \tparam ConcreteForcing The concrete derived class
     *
     * \see make_forcing(), ELASTICA_DEFINE_FORCING()
     */
    template <typename Registrars, typename ConcreteForcing>
    class Forcing : public detail::ForcingBase<Registrars>,
                    public CRTPHelper<ConcreteForcing, Forcing> {
     private:
      //**Type definitions******************************************************
      //! Type of parent
      using Parent = typename detail::ForcingBase<Registrars>;
      //! This type
      using This = Forcing<Registrars, ConcreteForcing>;
      //! CRTP Type
      using CRTP = CRTPHelper<ConcreteForcing, Forcing>;
      //! CRTP methods
      using CRTP::self;
      //************************************************************************

      /*
       * Not sure if this is a good idea : have a default empty implementation
       * here for apply_forces() and rates as well, so that you can early
       * exit the internal loop if needed (this makes it possible to efficiently
       * constrain only rates or values without spending empty iterations)
       */

     protected:
      //**Metafunctions*********************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\name Meta-programming functions
         Helper functions for metaprogramming constraints*/
      //@{

      //************************************************************************
      /*!\brief Compile-time check for overriding force in derived
       *
       * \details
       * If the derived class has no apply_forces() member function,
       * meaning that there is no need to apply forces, we can simply skip
       * evaluation rather than spending time iterating over the indices. This
       * check for skipping can happen in compile-time and is facilitated by
       * this function
       *
       * \note
       * Also works for overloaded functions

       * \tparam TimeType Type of time
       * \tparam System Type of system
       *
       * \metareturns std::bool_constant
      */
      template <typename TimeType, typename System>
      static inline constexpr auto derived_has_apply_forces() noexcept {
        return ::tt::is_detected<detail::Signatures<TimeType, System>::
                                     template apply_forces_signature,
                                 ConcreteForcing>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for no overriding force in derived
       *
       * \tparam TimeType Type of time
       * \tparam System Type of system
       *
       * \metareturns std::bool_constant
       *
       * \see derived_has_apply_forces
       */
      template <typename TimeType, typename System>
      static inline constexpr auto derived_has_no_apply_forces() noexcept {
        //        return std::is_same<
        //            decltype(&This::template apply_forces<TimeType, System>),
        //            decltype(
        //                &ConcreteForcing::template apply_forces<TimeType,
        //                System>)>{};
        return std::negation<
            decltype(derived_has_apply_forces<TimeType, System>())>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for overriding torque in derived
       *
       * \details
       * If the derived class has no `apply_torques` member function,
       * meaning that there is no need to apply torques, we can simply skip
       * evaluation rather than spending time iterating over the indices. This
       * check for skipping can happen in compile-time and is facilitated by
       * this function
       *
       * \note
       * Also works for overloaded functions
       *
       * \tparam TimeType Type of time
       * \tparam System Type of system
       *
       * \metareturns std::bool_constant
       */
      template <typename TimeType, typename System>
      static inline constexpr auto derived_has_apply_torques() noexcept {
        return ::tt::is_detected<detail::Signatures<TimeType, System>::
                                     template apply_torques_signature,
                                 ConcreteForcing>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for no overriding torque  in derived
       *
       * \tparam TimeType Type of time
       * \tparam System Type of system
       *
       * \metareturns std::bool_constant
       *
       * \see derived_has_apply_torques
       */
      template <typename TimeType, typename System>
      static inline constexpr auto derived_has_no_apply_torques() noexcept {
        return std::negation<
            decltype(derived_has_apply_torques<TimeType, System>())>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for a force function in derived optimized for
       * contiguous ranges
       *
       * \details
       * If the derived class has an `apply_forces_on_range` member function, we
       * assume that a data-parallel, optimized routine is provided. In that
       * case, when we have a contiguous range we use that member function
       * instead of the default apply_forces() method. In case the derived class
       * does not provide an optimized routine, we fall back to the default
       * function. This check for an optimized routine can happen in
       * compile-time and is facilitated by this function
       *
       * \note
       * This method does NOT need template, but without templates, its
       * instantiated when the class is instantiated. Since ConcreteForcing's
       * definition is incomplete in that time, this always results in false.
       * So to delay specialization, we introduce the System template here.
       *
       * \tparam TimeType Type of time
       * \tparam System Type of system
       *
       * \metareturns std::bool_constant
       */
      template <typename TimeType, typename System>
      static inline constexpr auto
      derived_has_apply_forces_on_range() noexcept {
        return ::tt::is_detected<detail::Signatures<TimeType, System>::
                                     template apply_forces_on_range_signature,
                                 ConcreteForcing>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for ranged force function in derived
       *
       * \tparam TimeType Type of time
       * \tparam System Type of system
       *
       * \metareturns std::bool_constant
       *
       * \see derived_has_apply_forces_on_range
       */
      template <typename TimeType, typename System>
      static inline constexpr auto
      derived_has_no_apply_forces_on_range() noexcept {
        return std::negation<
            decltype(derived_has_apply_forces_on_range<TimeType, System>())>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for a torque function in derived optimized
       * for contiguous ranges
       *
       * \details
       * If the derived class has an `apply_torques_on_range` member function,
       * we assume that a data-parallel, optimized routine is provided. In that
       * case, when we have a contiguous range we use that member function
       * instead of the default apply_torques() method. In case the derived
       * class does not provide an optimized routine, we fall back to the
       * default function. This check for an optimized routine can happen in
       * compile-time and is facilitated by this function
       *
       * \tparam TimeType Type of time
       * \tparam System Type of system
       *
       * \metareturns std::bool_constant
       */
      template <typename TimeType, typename System>
      static inline constexpr auto
      derived_has_apply_torques_on_range() noexcept {
        return ::tt::is_detected<detail::Signatures<TimeType, System>::
                                     template apply_torques_on_range_signature,
                                 ConcreteForcing>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for ranged torque function in derived
       *
       * \tparam TimeType Type of time
       * \tparam System Type of system
       *
       * \metareturns std::bool_constant
       *
       * \see derived_has_apply_torques_on_range
       */
      template <typename TimeType, typename System>
      static inline constexpr auto
      derived_has_no_apply_torques_on_range() noexcept {
        return std::negation<
            decltype(derived_has_apply_torques_on_range<TimeType, System>())>{};
      }
      //************************************************************************

      //@}
      /*! \endcond */
      //************************************************************************

     private:
      //**Type definitions******************************************************
      //! Type marking ConcreteForcing having a ranged implementation
      using ranged_tag = std::true_type;
      //! Type marking ConcreteForcing not having a ranged implementation
      using no_ranged_tag = std::false_type;
      //************************************************************************

     protected:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      //************************************************************************
      /*!\brief Inherited constructor
       */
      using Parent::Parent;
      //@}
      //************************************************************************

      //**Assignment Operators**************************************************
      /*!\name Assignment Operators */
      //@{
      //************************************************************************
      /*!\brief Inherited move assignment operator
       */
      using Parent::operator=;
      //************************************************************************
      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{
      virtual ~Forcing() = default;
      //@}
      //************************************************************************

     protected:
      //**User functions*******************************************************
      /*!\name User functions
         Functions to be overriden by the user*/
      //@{

      //************************************************************************
      /*!\brief The apply_forces() to be overriden by the user
       *
       * \return void
       */
      template <typename TimeType, typename System>
      void apply_forces(TimeType /* time */, System& /* system */,
                        std::size_t /* index */) const;
      //************************************************************************

      //************************************************************************
      /*!\brief The apply_torques() to be overriden by the user
       *
       * \return void
       */
      template <typename TimeType, typename System>
      void apply_torques(TimeType /* time */, System& /* system */,
                         std::size_t /* index */) const;
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Forcing functions*****************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\name Forcing functions */
      //@{

      /*!\name Force apply functions */
      //@{

      //************************************************************************
      /*!\brief Apply forces
       *
       * \details
       * Overloaded compile-time variant for a couple of indices
       */
      template <
          typename TimeType, typename System, std::uint8_t N,
          Requires<derived_has_apply_forces<TimeType, System>()> = nullptr>
      auto apply_forces_internal(
          TimeType t, System& sys,
          ::elastica::detail::numeric_at_t<N> const& index_set) const -> void {
        // unroll N times here
        index_apply<N>([&](auto... Is) {
          EXPAND_PACK_LEFT_TO_RIGHT(
              self().apply_forces(t, sys, std::get<Is>(index_set.args)));
        });
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply forces
       *
       * \details
       * Overloaded compile-time variant for a couple of indices
       */
      template <
          typename TimeType, typename System, std::uint8_t N,
          Requires<derived_has_no_apply_forces<TimeType, System>()> = nullptr>
      inline constexpr auto apply_forces_internal(
          TimeType, System&,
          ::elastica::detail::numeric_at_t<N> const&) const noexcept -> void {
        using Feedback IGNORE_UNUSED = decltype(
            detail::warn_on_finding_no_suitable_force_method<ConcreteForcing,
                                                             System>{});
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply forces
       *
       * \details
       * Overloaded runtime variant for a couple of indices
       */
      template <
          typename TimeType, typename System,
          Requires<derived_has_apply_forces<TimeType, System>()> = nullptr>
      auto apply_forces_internal(TimeType t, System& sys,
                                 const std::size_t* const index_data,
                                 std::size_t n_indices) const -> void {
        for (std::size_t i = 0; i < n_indices; ++i) {
          self().apply_forces(t, sys, index_data[i]);
        }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply forces
       *
       * \details
       * Overloaded runtime variant for a couple of indices
       */
      template <
          typename TimeType, typename System,
          Requires<derived_has_no_apply_forces<TimeType, System>()> = nullptr>
      inline constexpr auto apply_forces_internal(TimeType, System&,
                                                  const std::size_t* const,
                                                  std::size_t) const noexcept
          -> void {
        using Feedback IGNORE_UNUSED = decltype(
            detail::warn_on_finding_no_suitable_force_method<ConcreteForcing,
                                                             System>{});
      }
      //************************************************************************

     private:
      //************************************************************************
      /*!\brief Apply forces implementation with range
       *
       * \details
       * Overloaded runtime variant for a range of indices
       */
      template <typename TimeType, typename System>
      inline auto apply_forces_internal_impl(TimeType t, System& sys,
                                             std::size_t start,
                                             std::size_t stop, std::size_t step,
                                             ranged_tag /* meta */) const
          -> void {
        if (step == 1) {
          self().apply_forces_on_range(t, sys, start, stop);
        } else {
          apply_forces_internal_impl(t, sys, start, stop, step,
                                     no_ranged_tag{});
        }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply forces implementation with discontinuous range
       *
       * \details
       * Overloaded runtime variant for a range of indices
       */
      template <typename TimeType, typename System>
      inline auto apply_forces_internal_impl(TimeType t, System& sys,
                                             std::size_t start,
                                             std::size_t stop, std::size_t step,
                                             no_ranged_tag /* meta */) const
          -> void {
        for (std::size_t idx = start; idx < stop; idx += step) {
          self().apply_forces(t, sys, idx);
        }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Static check for whether derived has an apply_forces_on_range()
       * method without providing the regular apply_forces() method
       */
      template <
          typename TimeType, typename System,
          bool B = derived_has_no_apply_forces_on_range<TimeType, System>()>
      static inline constexpr auto check_for_incomplete_apply_forces() -> void {
        static_assert(B,
                      R"error(
You have chosen to provide a apply_forces_on_range() member function, but not
an apply_forces() member function. This is incompatible with applying forces on
a compact set of disconnected indices! Please provide an apply_forces() member
function as well, or rethink the logic of the constraint.
apply_forces() generally has the following signature:

// TODO : finalize based on API
template <typename TimeType, typename System>
void apply_forces(System& system, TimeType t, int index) const {
  // do something
})error");
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Apply forces
       *
       * \details
       * Overloaded runtime variant for a range of indices
       *
       * \note
       * This is a SFINAE specialization for when derived does not provide a
       * apply_forces method. This is required (a simple if like other routines
       * does not suffice) because we embed some additional type-safety logic
       * into the routine
       */
      template <
          typename TimeType, typename System,
          Requires<derived_has_no_apply_forces<TimeType, System>()> = nullptr>
      inline constexpr auto apply_forces_internal(
          TimeType /*t*/, System& /*sys*/,
          ::elastica::detail::NumericRange const& /*range*/) const noexcept
          -> void {
        check_for_incomplete_apply_forces<TimeType, System>();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply forces
       *
       * \details
       * Overloaded runtime variant for a range of indices
       *
       * \note
       * This is a SFINAE specialization for when derived provides a
       * apply_forces method. We can then use this method or the ranges method
       * as and when we see fit.
       */
      template <
          typename TimeType, typename System,
          Requires<derived_has_apply_forces<TimeType, System>()> = nullptr>
      auto apply_forces_internal(
          TimeType t, System& sys,
          ::elastica::detail::NumericRange const& range) const -> void {
        const auto start = range.start_;
        const auto stop = range.stop_;
        const auto step = range.step_;
        // tag dispatch
        apply_forces_internal_impl(
            t, sys, start, stop, step,
            derived_has_apply_forces_on_range<TimeType, System>());
      }
      //************************************************************************
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Torque apply functions */
      //@{

      //************************************************************************
      /*!\brief Apply torques
       *
       * \details
       * Overloaded compile-time variant for a couple of indices
       */
      template <
          typename TimeType, typename System, std::uint8_t N,
          Requires<derived_has_apply_torques<TimeType, System>()> = nullptr>
      auto apply_torques_internal(
          TimeType t, System& sys,
          ::elastica::detail::numeric_at_t<N> const& index_set) const -> void {
        // early exit in a constexpr fashion
        // Unroll N times here
        index_apply<N>([&](auto... Is) {
          EXPAND_PACK_LEFT_TO_RIGHT(
              self().apply_torques(t, sys, std::get<Is>(index_set.args)));
        });
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply torques
       *
       * \details
       * Overloaded compile-time variant for a couple of indices
       */
      template <
          typename TimeType, typename System, std::uint8_t N,
          Requires<derived_has_no_apply_torques<TimeType, System>()> = nullptr>
      inline constexpr auto apply_torques_internal(
          TimeType, System&,
          ::elastica::detail::numeric_at_t<N> const&) const noexcept -> void {
        using Feedback IGNORE_UNUSED = decltype(
            detail::warn_on_finding_no_suitable_torque_method<ConcreteForcing,
                                                              System>{});
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply torques
       *
       * \details
       * Overloaded runtime variant for a couple of indices
       */
      template <
          typename TimeType, typename System,
          Requires<derived_has_apply_torques<TimeType, System>()> = nullptr>
      auto apply_torques_internal(TimeType t, System& sys,
                                  const std::size_t* const index_data,
                                  std::size_t n_indices) const -> void {
        for (std::size_t i = 0; i < n_indices; ++i) {
          self().apply_torques(t, sys, index_data[i]);
        }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply torques
       *
       * \details
       * Overloaded runtime variant for a couple of indices
       */
      template <
          typename TimeType, typename System,
          Requires<derived_has_no_apply_torques<TimeType, System>()> = nullptr>
      inline constexpr auto apply_torques_internal(
          TimeType /* t */, System& /*sys*/,
          const std::size_t* const /*index_data*/, std::size_t /*n_indices*/
      ) const noexcept -> void {
        using Feedback IGNORE_UNUSED = decltype(
            detail::warn_on_finding_no_suitable_torque_method<ConcreteForcing,
                                                              System>{});
      }
      //************************************************************************

     private:
      //************************************************************************
      /*!\brief Apply torques implementation with range
       *
       * \details
       * Overloaded runtime variant for a range of indices
       */
      template <typename TimeType, typename System>
      inline auto apply_torques_internal_impl(TimeType t, System& sys,
                                              std::size_t start,
                                              std::size_t stop,
                                              std::size_t step,
                                              ranged_tag /* meta */) const
          -> void {
        if (step == 1) {
          self().apply_torques_on_range(t, sys, start, stop);
        } else {
          apply_torques_internal_impl(t, sys, start, stop, step,
                                      no_ranged_tag{});
        }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply torques implementation with discontinuous range
       *
       * \details
       * Overloaded runtime variant for a range of indices
       */
      template <typename TimeType, typename System>
      inline auto apply_torques_internal_impl(TimeType t, System& sys,
                                              std::size_t start,
                                              std::size_t stop,
                                              std::size_t step,
                                              no_ranged_tag /* meta */) const
          -> void {
        for (auto idx = start; idx < stop; idx += step) {
          self().apply_torques(t, sys, idx);
        }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Static check for whether derived has an apply_torques_on_range()
       * method without providing the regular apply_torques() method
       */
      template <
          typename TimeType, typename System,
          bool B = derived_has_no_apply_torques_on_range<TimeType, System>()>
      static inline constexpr auto check_for_incomplete_apply_torques() noexcept
          -> void {
        static_assert(B,
                      R"error(
You have chosen to provide a apply_torques_on_range() member function, but not
an apply_torques() member function. This is incompatible with applying torques on
a compact set of disconnected indices! Please provide an apply_torques() member
function as well, or rethink the logic of the constraint.
apply_torques() generally has the following signature:

// TODO : finalize based on API
template <typename TimeType, typename System>
void apply_torques(System& system, TimeType t, int index) const {
  // do something
})error");
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Apply torques
       *
       * \details
       * Overloaded runtime variant for a range of indices
       *
       * \note
       * This is a SFINAE specialization for when derived does not provide a
       * apply_forces method. This is required (a simple if like other routines
       * does not suffice) because we embed some additional type-safety logic
       * into the routine
       */
      template <
          typename TimeType, typename System,
          Requires<derived_has_no_apply_torques<TimeType, System>()> = nullptr>
      inline constexpr auto apply_torques_internal(
          TimeType /*t*/, System& /*sys*/,
          ::elastica::detail::NumericRange const& /*range*/) const noexcept
          -> void {
        check_for_incomplete_apply_torques<TimeType, System>();
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply torques
       *
       * \details
       * Overloaded runtime variant for a range of indices
       *
       * \note
       * This is a SFINAE specialization for when derived provides a
       * apply_forces method. We can then use this method or the ranges method
       * as and when we see fit.
       */
      template <
          typename TimeType, typename System,
          Requires<derived_has_apply_torques<TimeType, System>()> = nullptr>
      auto apply_torques_internal(
          TimeType t, System& sys,
          ::elastica::detail::NumericRange const& range) const -> void {
        const auto start = range.start_;
        const auto stop = range.stop_;
        const auto step = range.step_;
        apply_torques_internal_impl(
            t, sys, start, stop, step,
            derived_has_apply_torques_on_range<TimeType, System>());
      }
      //************************************************************************
      //@}
      //************************************************************************

      //@}
      /*! \endcond */
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace forcing

}  // namespace elastica
