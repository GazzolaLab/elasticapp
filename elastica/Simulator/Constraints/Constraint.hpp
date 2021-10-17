//==============================================================================
/*!
//  \file
//  \brief Base class for all constraints
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
#include "Simulator/Constraints/Types.hpp"
///
#include "Simulator/Constraints/Protocols.hpp"
#include "Simulator/Constraints/InitialConditionsCache.hpp"
///
#include "Utilities/CRTP.hpp"
#include "Utilities/FakeVirtual.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/NoSuchType.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/NumericAt.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/Registration.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace elastica {

  namespace constraints {

    namespace detail {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Abstract classes for templated constraints within \elastica
       * \ingroup constraints
       */
      DEFINE_FAKE_VIRTUAL(constrain_primitives_internal)
      DEFINE_FAKE_VIRTUAL(constrain_rates_internal)
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Abstract class for storing constraints within \elastica
       * \ingroup constraints
       *
       * \details
       * ConstraintBase is an abstract class to access constraints during
       * runtime in \elastica.
       *
       * \tparam Registrars Registration parameter
       */
      template <typename Registrars>
      class ConstraintBase
          : public NonCopyable,
            public ::tt::ConformsTo<
                ::elastica::constraints::protocols::Constraint> {
       public:
        //**Type definitions****************************************************
        //! Inherit type, all derived classes must inherit from this type
        using Inherit = FakeVirtualInherit_constrain_rates_internal<
            FakeVirtualInherit_constrain_primitives_internal<
                ConstraintBase<Registrars>>>;

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
        ConstraintBase() noexcept = default;
        //**********************************************************************

        // explicitly declare the move constructors for safety

        //**********************************************************************
        /*!\brief The move constructor.
         */
        ConstraintBase(ConstraintBase&&) noexcept = default;
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Assignment Operators************************************************
        /*!\name Assignment Operators */
        //@{

        //**********************************************************************
        /*!\brief The move assignment operator.
         */
        ConstraintBase& operator=(ConstraintBase&&) noexcept = default;
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
        virtual ~ConstraintBase() = default;
        //@}
        //**********************************************************************

        //**Constraint functions************************************************
        /*!\name Constraint functions */
        //@{

        /*!\name Value constraint functions */
        //@{

        //**********************************************************************
        /*!\brief Constrain primitives
         *
         * \details
         * Overloaded compile-time variant
         */
        template <typename TimeType, typename System, std::uint8_t N>
        auto constrain_primitives_internal(
            TimeType t, System& sys,
            elastica::detail::numeric_at_t<N> const& index_set) const -> void {
          return fake_virtual_constrain_primitives_internal<creatable_classes>(
              this, t, sys, index_set);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Constrain primitives
         *
         * \details
         * Overloaded runtime variant
         */
        template <typename TimeType, typename System>
        auto constrain_primitives_internal(TimeType t, System& sys,
                                           const std::size_t* const index_data,
                                           std::size_t n_indices) const
            -> void {
          return fake_virtual_constrain_primitives_internal<creatable_classes>(
              this, t, sys, index_data, n_indices);
        }
        //**********************************************************************

        //@}
        //**********************************************************************
        /*!\name Rate constraint functions */
        //@{

        //**********************************************************************
        /*!\brief Constrain rates
         *
         * \details
         * Overloaded compile-time variant
         */
        template <typename TimeType, typename System, std::uint8_t N>
        auto constrain_rates_internal(
            TimeType t, System& sys,
            elastica::detail::numeric_at_t<N> const& index_set) const -> void {
          return fake_virtual_constrain_rates_internal<creatable_classes>(
              this, t, sys, index_set);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Constrain rates
         *
         * \details
         * Overloaded runtime variant
         */
        template <typename TimeType, typename System>
        auto constrain_rates_internal(TimeType t, System& sys,
                                      const std::size_t* const index_data,
                                      std::size_t n_indices) const -> void {
          return fake_virtual_constrain_rates_internal<creatable_classes>(
              this, t, sys, index_data, n_indices);
        }
        //**********************************************************************
        //@}
        //**********************************************************************

        //@}
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Signature of expected constraints
       * \ingroup constraints
       *
       * Very similar to protocols, but we don't need to derive from the
       * protocol class. Rather, signatures rely on an implicit interface
       * guarantee.
       *
       * \tparam TimeType Type of time
       * \tparam System Type of system
       */
      template <typename TimeType, typename System>
      struct Signatures {
        /// [constrain_primitives_signature]
        template <typename Constraint>
        using constrain_primitives_signature =
            decltype(std::declval<Constraint const&>().constrain_primitives(
                std::declval<TimeType>(), std::declval<System&>(),
                std::declval<std::size_t>()));
        // This is equivalent to
        //   template <typename TimeType, typename System>
        //   void constrain_primitives(TimeType t, System& sys, std::size_t
        //   index) const {
        //     // do something
        //   }
        /// [constrain_primitives_signature]

        /// [constrain_rates_signature]
        template <typename Constraint>
        using constrain_rates_signature =
            decltype(std::declval<Constraint const&>().constrain_rates(
                std::declval<TimeType>(), std::declval<System&>(),
                std::declval<std::size_t>()));
        // This is equivalent to
        //   template <typename TimeType, typename System>
        //   void constrain_rates(TimeType t, System& sys, std::size_t index)
        //   const {
        //     // do something
        //   }
        /// [constrain_rates_signature]
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Warn user on finding no suitable methods
       * \ingroup constraints
       */
      template <typename Constreint, typename System>
      struct [[deprecated(R"warn(
The current Constraint (first template parameter) has no constrain_primitives method
that can act on the given System (second template parameter)
)warn")]] warn_on_finding_no_suitable_primitives_method{};
      template <typename Constreint, typename System>
      struct [[deprecated(R"warn(
The current Constraint (first template parameter) has no constrain_rates method
that can act on the given System (second template parameter)
)warn")]] warn_on_finding_no_suitable_rates_method{};
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief Default implementation of constraints in \elastica
     * \ingroup constraints
     *
     * \details
     * Constraint implements the concept of a constraint/boundary condition that
     * reduces/imposes constraints on various degrees of freedom of an
     * \elastica entity (such as rods, rigid bodies etc.). Any valid constraint
     * in \elastica needs to derive from this class using the CRTP pattern, as
     * shown in the example below (or alternatively use the
     * ELASTICA_DEFINE_CONSTRAINT() macro). A constraint can choose to constrain
     * value types (such as positions/directors) using the
     * constrain_primitives() member function and/or rates (such as velocities
     * and angular velocities) using the constrain_rates() member function.
     * Optionally we can choose not to define one of the above functions in
     * which case we assume a no-operation.
     *
     * \usage
     * The expected prototype of these two methods are shown below:
     *
     * \snippet this constrain_primitives_signature
     *
     * where
     * - `t` is the current time of the simulation
     * - `sys` is an object modeling an \elastica system
     * - `index` is the index being constrained.
     *
     * And the rates method has the following prototype:
     *
     * \snippet this constrain_rates_signature
     *
     * where
     * - `t` is the current time of the simulation
     * - `sys` is an object modeling an \elastica system
     * - `index` is the index being constrained.
     *
     * We suggest the user to place all constraints within the
     * elastica::constraints namespace as shown in the examples below.
     * Additionally, we provide the helpful macros ELASTICA_DEFINE_CONSTRAINT()
     * and ELASTICA_REGISTER_CONSTRAINT() for reducing boiler-plate code. See
     * their documentations, and the examples below, for more details on
     * creating constraints.
     *
     * Finally, for additional convenience, we provide the
     * elastica::constraints::make_constraint() function that
     * eases creation of a Constraint in \elastica, and we suggest the user to
     * consult its documentation first.
     *
     * \example
     * A valid constraint with both functions specified is shown below.
     *
     * \snippet Simulator/Constraints/Test_Constraint.cpp cwam
     *
     * Alternatively you can chose to specify either one of them, such as
     * constrain_primitives()
     *
     * \snippet Simulator/Constraints/Test_Constraint.cpp cwvo
     *
     * or constrain_rates()
     *
     * \snippet Simulator/Constraints/Test_Constraint.cpp cwro
     *
     * in which case a no-operation is assumed for the other function. It is
     * also possible to overload the functions for specific types, shown below
     * for constrain_primitives()
     *
     * \snippet Simulator/Constraints/Test_Constraint.cpp cwsv
     *
     * or even templated types
     *
     * \snippet Simulator/Constraints/Test_Constraint.cpp cwtv
     *
     * which proves useful for metaprogramming based on the type-category of a
     * \elastica system (such as a Cosserat rod or a rigid body etc.), because
     * systems in \elastica are implemented using static polymorphism following
     * the CRTP pattern
     *
     * \code
     *  template <typename TimeType, typename T>
     *  constrain_primitives(TimeType t, RigidBody<T>&, ...);
     *
     *  template <typename TimeType, typename T>
     *  constrain_primitives(TimeType t, RodType<T>&, ...);
     * \endcode
     *
     * \tparam Registrars Registrars of derived classes, see Registration
     * \tparam ConcreteConstraint The concrete derived class
     *
     * \see make_constraint(), ELASTICA_DEFINE_CONSTRAINT()
     */
    template <typename Registrars, typename ConcreteConstraint>
    class Constraint : public detail::ConstraintBase<Registrars>::Inherit,
                       public CRTPHelper<ConcreteConstraint, Constraint> {
     private:
      //**Type definitions******************************************************
      //! Type of parent
      using Parent = typename detail::ConstraintBase<Registrars>::Inherit;
      //! This type
      using This = Constraint<Registrars, ConcreteConstraint>;
      //! CRTP Type
      using CRTP = CRTPHelper<ConcreteConstraint, Constraint>;
      //! CRTP methods
      using CRTP::self;
      //************************************************************************

      /*
       * Not sure if this is a good idea : have a default empty implementation
       * here for constrain_primitives and rates as well, so that you can early
       * exit the internal loop if needed (this makes it possible to efficiently
       * constrain only rates or primitives without spending empty iterations)
       */

     protected:
      //**Metafunctions*********************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\name Meta-programming functions
         Helper functions for metaprogramming constraints*/
      //@{

      //************************************************************************
      /*!\brief Compile-time check for overriding value constraint in derived
       *
       * \details
       * If the derived class has no constrain_primitives() member function,
       * meaning that there is no need to constrain primitives, we can simply
       * skip evaluation rather than spending time iterating over the indices.
       * This check for skipping can happen in compile-time and is facilitated
       * by this function.
       *
       * \note
       * Also works for overloaded functions
       *
       * \tparam TimeType Type of time
       * \tparam System Type of system
       *
       * \metareturns cpp17::bool_constant
       */
      template <typename TimeType, typename System>
      static inline constexpr auto derived_has_constrain_primitives() noexcept {
        return ::tt::is_detected<detail::Signatures<TimeType, System>::
                                     template constrain_primitives_signature,
                                 ConcreteConstraint>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for no overriding value constraint in derived
       *
       * \tparam TimeType Type of time
       * \tparam System Type of system
       *
       * \metareturns cpp17::bool_constant
       *
       * \see derived_has_constrain_primitives()
       */
      template <typename TimeType, typename System>
      static inline constexpr auto
      derived_has_no_constrain_primitives() noexcept {
        return cpp17::negation<
            decltype(derived_has_constrain_primitives<TimeType, System>())>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for overriding rate constraint in derived
       *
       * \details
       * If the derived class has no constrain_rates() member function,
       * meaning that there is no need to constrain rates, we can simply skip
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
       * \metareturns cpp17::bool_constant
       */
      template <typename TimeType, typename System>
      static inline constexpr auto derived_has_constrain_rates() noexcept {
        return ::tt::is_detected<detail::Signatures<TimeType, System>::
                                     template constrain_rates_signature,
                                 ConcreteConstraint>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for no overriding rate constraint in derived
       *
       * \tparam TimeType Type of time
       * \tparam System Type of system
       *
       * \metareturns cpp17::bool_constant
       *
       * \see derived_has_constrain_rates()
       */
      template <typename TimeType, typename System>
      static inline constexpr auto derived_has_no_constrain_rates() noexcept {
        return cpp17::negation<
            decltype(derived_has_constrain_rates<TimeType, System>())>{};
      }
      //************************************************************************

      //@}
      /*! \endcond */
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
      virtual ~Constraint() = default;
      //@}
      //************************************************************************

     protected:
      //**User functions*******************************************************
      /*!\name User functions
         Functions to be overriden by the user*/
      //@{

      //************************************************************************
      /*!\brief The constrain_primitives() to be overriden by the user
       *
       * \return void
       */
      template <typename TimeType, typename System>
      void constrain_primitives(TimeType /* time */, System& /* system */,
                                std::size_t /* index */) const;
      //************************************************************************

      //************************************************************************
      /*!\brief The constrain_rates() to be overriden by the user
       *
       * \return void
       */
      template <typename TimeType, typename System>
      void constrain_rates(TimeType /* time */, System& /* system */,
                           std::size_t /* index */) const;
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Constraint functions**************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\name Constraint functions */
      //@{

      /*!\name Value constraint functions */
      //@{

      //************************************************************************
      /*!\brief Constrain primitives
       *
       * \details
       * Overloaded compile-time variant
       *
       */
      template <typename TimeType, typename System, std::uint8_t N,
                Requires<derived_has_constrain_primitives<TimeType, System>()> =
                    nullptr>
      auto constrain_primitives_internal(
          TimeType t, System& sys,
          elastica::detail::numeric_at_t<N> const& index_set) const -> void {
        // unroll N times here
        index_apply<N>([&](auto... Is) {
          EXPAND_PACK_LEFT_TO_RIGHT(self().constrain_primitives(
              t, sys, std::get<Is>(index_set.args)));
        });
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Constrain primitives
       *
       * \details
       * Overloaded compile-time variant
       *
       */
      template <
          typename TimeType, typename System, std::uint8_t N,
          Requires<derived_has_no_constrain_primitives<TimeType, System>()> =
              nullptr>
      inline constexpr auto constrain_primitives_internal(
          TimeType, System&,
          elastica::detail::numeric_at_t<N> const&) const noexcept -> void {
        using Feedback IGNORE_UNUSED =
            decltype(detail::warn_on_finding_no_suitable_primitives_method<
                     ConcreteConstraint, System>{});
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Constrain primitives
       *
       * \details
       * Overloaded runtime variant
       */
      template <typename TimeType, typename System,
                Requires<derived_has_constrain_primitives<TimeType, System>()> =
                    nullptr>
      auto constrain_primitives_internal(TimeType t, System& sys,
                                         const std::size_t* const index_data,
                                         std::size_t n_indices) const -> void {
        for (std::size_t i = 0; i < n_indices; ++i) {
          self().constrain_primitives(t, sys, index_data[i]);
        }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Constrain primitives
       *
       * \details
       * Overloaded runtime variant
       */
      template <
          typename TimeType, typename System,
          Requires<derived_has_no_constrain_primitives<TimeType, System>()> =
              nullptr>
      inline constexpr auto constrain_primitives_internal(
          TimeType, System&, const std::size_t* const, std::size_t) const
          -> void {
        using Feedback IGNORE_UNUSED =
            decltype(detail::warn_on_finding_no_suitable_primitives_method<
                     ConcreteConstraint, System>{});
      }
      //************************************************************************
      //@}
      //************************************************************************

      //************************************************************************
      /*!\name Rate constraint functions */
      //@{

      //************************************************************************
      /*!\brief Constrain rates
       *
       * \details
       * Overloaded compile-time variant
       */
      template <
          typename TimeType, typename System, std::uint8_t N,
          Requires<derived_has_constrain_rates<TimeType, System>()> = nullptr>
      auto constrain_rates_internal(
          TimeType t, System& sys,
          elastica::detail::numeric_at_t<N> const& index_set) const -> void {
        // Unroll N times here
        index_apply<N>([&](auto... Is) {
          EXPAND_PACK_LEFT_TO_RIGHT(
              self().constrain_rates(t, sys, std::get<Is>(index_set.args)));
        });
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Constrain rates
       *
       * \details
       * Overloaded compile-time variant
       */
      template <typename TimeType, typename System, std::uint8_t N,
                Requires<derived_has_no_constrain_rates<TimeType, System>()> =
                    nullptr>
      inline constexpr auto constrain_rates_internal(
          TimeType, System&,
          elastica::detail::numeric_at_t<N> const&) const noexcept -> void {
        using Feedback IGNORE_UNUSED = decltype(
            detail::warn_on_finding_no_suitable_rates_method<ConcreteConstraint,
                                                             System>{});
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Constrain rates
       *
       * \details
       * Overloaded runtime variant
       */
      template <
          typename TimeType, typename System,
          Requires<derived_has_constrain_rates<TimeType, System>()> = nullptr>
      auto constrain_rates_internal(TimeType t, System& sys,
                                    const std::size_t* const index_data,
                                    std::size_t n_indices) const -> void {
        for (std::size_t i = 0; i < n_indices; ++i) {
          self().constrain_rates(t, sys, index_data[i]);
        }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Constrain rates
       *
       * \details
       * Overloaded runtime variant
       */
      template <typename TimeType, typename System,
                Requires<derived_has_no_constrain_rates<TimeType, System>()> =
                    nullptr>
      inline constexpr auto constrain_rates_internal(TimeType, System&,
                                                     const std::size_t* const,
                                                     std::size_t) const noexcept
          -> void {
        using Feedback IGNORE_UNUSED = decltype(
            detail::warn_on_finding_no_suitable_rates_method<ConcreteConstraint,
                                                             System>{});
      }
      //************************************************************************
      //@}
      //************************************************************************

      //@}
      /*! \endcond */
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace constraints

}  // namespace elastica
