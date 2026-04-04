//==============================================================================
/*!
//  \file
//  \brief Base class for all connections
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

#include <type_traits>

///
#include "Simulator/Connections/Types.hpp"
///
#include "Simulator/Connections/Protocols.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/FakeVirtual.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/NoSuchType.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/Registration.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/IsDetected.hpp"

namespace elastica {

  namespace connections {

    namespace detail {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Abstract class for storing connections within \elastica
       * \ingroup connections
       *
       * \details
       * ConnectionBase is an abstract class to access connections during
       * runtime in \elastica.
       *
       * \tparam Registrars Registration parameter
       */
      template <typename Registrars>
      class ConnectionBase
          : public NonCopyable,
            public ::tt::ConformsTo<
                ::elastica::connections::protocols::Connection> {
       private:
        //**Type definitions****************************************************
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
        ConnectionBase() noexcept = default;
        //**********************************************************************

        // explicitly declare the move constructors for safety

        //**********************************************************************
        /*!\brief The move constructor.
         */
        ConnectionBase(ConnectionBase&&) noexcept = default;
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Assignment Operators************************************************
        /*!\name Assignment Operators */
        //@{

        //**********************************************************************
        /*!\brief The move assignment operator.
         */
        ConnectionBase& operator=(ConnectionBase&&) noexcept = default;
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
        virtual ~ConnectionBase() = default;
        //@}
        //**********************************************************************

        //**Connection functions************************************************
        /*!\name Connection functions */
        //@{

        /*!\name Force apply functions */
        //@{

        //**********************************************************************
        /*!\brief Apply forces
         *
         * \details
         * Overloaded variant for non-deferred types
         */
        template <typename TimeType, typename FirstSystem,
                  typename SecondSystem>
        auto apply_forces_internal(TimeType t, FirstSystem& first_sys,
                                   std::size_t first_index,
                                   SecondSystem& second_sys,
                                   std::size_t second_index) const -> void {
          return call_with_dynamic_type<void, creatable_classes>(
              this, [&](auto* const connexion) {
                return connexion->apply_forces_internal(
                    t, first_sys, first_index, second_sys, second_index);
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply forces
         *
         * \details
         * Overloaded variant for deferred types
         */
        template <typename TimeType, typename FirstSystem,
                  typename SecondSystem>
        auto apply_forces_internal(TimeType t, FirstSystem& first_sys,
                                   SecondSystem& second_sys) const -> void {
          return call_with_dynamic_type<void, creatable_classes>(
              this, [&](auto* const connexion) {
                return connexion->apply_forces_internal(t, first_sys,
                                                        second_sys);
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
         * Overloaded compile-time variant
         */
        template <typename TimeType, typename FirstSystem,
                  typename SecondSystem>
        auto apply_torques_internal(TimeType t, FirstSystem& first_sys,
                                    std::size_t first_index,
                                    SecondSystem& second_sys,
                                    std::size_t second_index) const -> void {
          return call_with_dynamic_type<void, creatable_classes>(
              this, [&](auto* const connexion) {
                return connexion->apply_torques_internal(
                    t, first_sys, first_index, second_sys, second_index);
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply torques
         *
         * \details
         * Overloaded runtime variant
         */
        template <typename TimeType, typename FirstSystem,
                  typename SecondSystem>
        auto apply_torques_internal(TimeType t, FirstSystem& first_sys,
                                    SecondSystem& second_sys) const -> void {
          return call_with_dynamic_type<void, creatable_classes>(
              this, [&](auto* const connexion) {
                return connexion->apply_torques_internal(t, first_sys,
                                                         second_sys);
              });
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
      /*!\brief Signature of expected connections
       * \ingroup connections
       *
       * Very similar to protocols, but we don't need to derive from the
       * protocol class. Rather, signatures rely on an implicit interface
       * guarantee. Also similar to is_callable/is_invokable, but its better
       * to have an explicit Signatures class.
       *
       * \tparam TimeType Type of time
       * \tparam FirstSystem Type of first system
       * \tparam SecondSystem Type of second system
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      struct Signatures {
        /// [apply_forces_signature]
        template <typename Connection>
        using apply_forces_signature =
            decltype(std::declval<Connection const&>().apply_forces(
                std::declval<TimeType>(), std::declval<FirstSystem&>(),
                std::declval<std::size_t>(), std::declval<SecondSystem&>(),
                std::declval<std::size_t>()));
        /* // This is equivalent to
         * template <typename TimeType, typename FirstSystem,
         *           typename SecondSystem>
         * void apply_forces(TimeType t, FirstSystem& first_sys,
         * std::size_t first_index,  SecondSystem& second_sys, std::size_t
         * second_index) const {
         *    // do something
         * }
         */
        /// [apply_forces_signature]

        /// [deferred_apply_forces_signature]
        template <typename Connection>
        using deferred_apply_forces_signature =
            decltype(std::declval<Connection const&>().apply_forces(
                std::declval<TimeType>(), std::declval<FirstSystem&>(),
                std::declval<SecondSystem&>()));
        /* // This is equivalent to
         * template <typename TimeType, typename FirstSystem,
         *           typename SecondSystem>
         * void apply_forces(TimeType t, FirstSystem& first_sys,
         * SecondSystem& second_sys) const {
         *    // do something
         * }
         */
        /// [deferred_apply_forces_signature]

        // for lambdas in Generic Connection
        template <typename Connection>
        using nonwrapped_signature =
            decltype(std::declval<Connection const&>().operator()(
                std::declval<TimeType>(), std::declval<FirstSystem&>(),
                std::declval<std::size_t>(), std::declval<SecondSystem&>(),
                std::declval<std::size_t>()));
        template <typename Connection>
        using nonwrapped_deferred_signature =
            decltype(std::declval<Connection const&>().operator()(
                std::declval<TimeType>(), std::declval<FirstSystem&>(),
                std::declval<SecondSystem&>()));

        /// [apply_torques_signature]
        template <typename Connection>
        using apply_torques_signature =
            decltype(std::declval<Connection const&>().apply_torques(
                std::declval<TimeType>(), std::declval<FirstSystem&>(),
                std::declval<std::size_t>(), std::declval<SecondSystem&>(),
                std::declval<std::size_t>()));
        /* // This is equivalent to
         * template <typename TimeType, typename FirstSystem,
         *            typename SecondSystem>
         * void apply_torques(TimeType t, FirstSystem& first_sys,
         * std::size_t first_index,  SecondSystem& second_sys, std::size_t
         * second_index) const {
         *    // do something
         * }
         */
        /// [apply_torques_signature]

        /// [deferred_apply_torques_signature]
        template <typename Connection>
        using deferred_apply_torques_signature =
            decltype(std::declval<Connection const&>().apply_torques(
                std::declval<TimeType>(), std::declval<FirstSystem&>(),
                std::declval<SecondSystem&>()));
        /* // This is equivalent to
         * template <typename TimeType, typename FirstSystem,
         *            typename SecondSystem>
         * void apply_torques(TimeType t, FirstSystem& first_sys,
         * SecondSystem& second_sys) const {
         *    // do something
         * }
         */
        /// [deferred_apply_torques_signature]
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Warn user on finding no suitable methods
       * \ingroup connections
       */
      template <typename Connecxion, typename FirstSystem,
                typename SecondSystem>
      struct [[deprecated(R"warn(
The current Connection (first template parameter) has no apply_forces() that can
act on the given systems FirstSystem (second template parameter) and SecondSystem
(third template parameter)
)warn")]] warn_on_finding_no_suitable_force_method{};
      template <typename Connecxion, typename FirstSystem,
                typename SecondSystem>
      struct [[deprecated(R"warn(
The current Connection (first template parameter) has no apply_torques() that can
act on the given systems FirstSystem (second template parameter) and SecondSystem
(third template parameter)
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
    /*!\brief Default implementation of connections in \elastica
     * \ingroup connections
     *
     * \details
     * Connection implements the concept of a connection that connects different
     * \elastica entities (such as rods, rigid bodies etc.) to one another.
     * Any valid connection in \elastica needs to derive from this class using
     * the CRTP pattern shown in the example below (or alternatively using the
     * ELASTICA_DEFINE_CONNECTION() macro).
     * A connection can choose to apply equal and opposite forces on the systems
     * using the apply_forces() member function and/or equal and opposite
     * torques using the apply_torques() member function. Optionally we can
     * choose not to define one of the above functions in which case we assume
     * a no-operation.
     *
     * \usage
     * The prototype of these two methods are shown below:
     *
     * \snippet this apply_forces_signature
     *
     * where `first_sys` and `second_sys` are objects modeling \elastica system,
     * `t` is the current time of the simulation, `first_index` and
     * `second_index` are the indices being connected on the two different
     * systems respectively.
     * And the torques method has the following prototype:
     *
     * \snippet this apply_torques_signature
     *
     * where `first_sys` and `second_sys` are objects modeling \elastica system,
     * `t` is the current time of the simulation, `first_index` and
     * `second_index` are the indices being connected on the two different
     * systems respectively.
     *
     * We suggest the user to place all connections within the
     * elastica::connections namespace as shown in the examples below.
     * Additionally, we provide the helpful macros ELASTICA_DEFINE_CONNECTION()
     * and ELASTICA_REGISTER_CONNECTION() for reducing boiler-plate code. See
     * their documentation, and the examples below, for more details on creating
     * connections.
     *
     * Finally, for additional convenience, we provide the
     * elastica::connections::make_connection() function that
     * eases creation of a Connection in \elastica, and we suggest the user to
     * consult its documentation first.
     *
     * \example
     * A valid connection with both functions specified is shown below.
     * \snippet Simulator/Connections/Test_Connection.cpp cwam
     *
     * Alternatively you can chose to specify either one of them, such as
     * apply_forces()
     *
     * \snippet Simulator/Connections/Test_Connection.cpp cwfo
     *
     * or apply_torques()
     *
     * \snippet Simulator/Connections/Test_Connection.cpp cwto
     *
     * in which case a no-operation is assumed for the other function. It is
     * also possible to overload the functions for specific types, shown below
     * for apply_forces()
     *
     * \snippet Simulator/Connections/Test_Connection.cpp cwsf
     *
     * or even templated types
     *
     * \snippet Simulator/Connections/Test_Connection.cpp cwtf
     *
     * which proves useful for metaprogramming based on the type-category of a
     * \elastica system (such as a Cosserat rod or a rigid body etc.), because
     * systems in \elastica are implemented using static polymorphism following
     * the CRTP pattern
     *
     * \code
     * template <typename TimeType, typename T>
     * apply_forces(TimeType t, RigidBody<T>&, ...);
     *
     * template <typename TimeType, typename T>
     * apply_forces(TimeType t, RodType<T>&, ...);
     * \endcode
     *
     * Additionally, in case of distributed connectinos acting over a range of
     * indices, it is not possible to determine apriori a range or how the
     * connections in the two systems will be mapped (do we do an n^2 iteration
     * over the indices belonging to both systems, which seems unlikely? Or do
     * we do a one-to-one mapping between the indices). Hence we leave the
     * decision to the user on how best to iterate over these systems. In such
     * cases, one should provide and use the modified methods whose prototypes
     * are shown below.
     *
     * \snippet this deferred_apply_forces_signature
     * \snippet this deferred_apply_torques_signature
     *
     * where `first_sys` and `second_sys` are objects modeling \elastica system,
     * and `t` is the current time of the simulation (note the absence of the
     * indices in particular). An example of such a connection is shown below
     *
     * \snippet Simulator/Connections/Test_Connection.cpp dce
     *
     * The way these connections are called into our simulation is using the
     * `defer` object, like so
     *
     * \snippet Interface/Connections/Test_Connection.cpp dce
     *
     * \tparam Registrars Registrars of derived classes, \see Registration
     * \tparam ConcreteConnection The concrete derived class
     *
     * see elastica::connections::make_connection()
     */
    template <typename Registrars, typename ConcreteConnection>
    class Connection : public detail::ConnectionBase<Registrars>,
                       public CRTPHelper<ConcreteConnection, Connection> {
     private:
      //**Type definitions******************************************************
      //! Type of parent
      using Parent = typename detail::ConnectionBase<Registrars>;
      //! This type
      using This = Connection<Registrars, ConcreteConnection>;
      //! CRTP Type
      using CRTP = CRTPHelper<ConcreteConnection, Connection>;
      //! CRTP methods
      using CRTP::self;
      //************************************************************************

      /*
       * Not sure if this is a good idea : have a default empty implementation
       * here for apply_forces and rates as well, so that you can early
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
      /*!\brief Compile-time check for overriding force function in derived
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
       *
       * \tparam TimeType Type of time
       * \tparam FirstSystem Type of first system
       * \tparam SecondSystem Type of second system
       *
       * \metareturns ::std::bool_constant
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      static inline constexpr auto derived_has_apply_forces() noexcept {
        return ::tt::is_detected<
            detail::Signatures<TimeType, FirstSystem,
                               SecondSystem>::template apply_forces_signature,
            ConcreteConnection>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for overriding apply_forces() in derived
       *
       * \tparam TimeType Type of time
       * \tparam FirstSystem Type of first system
       * \tparam SecondSystem Type of second system
       *
       * \metareturns std::bool_constant
       *
       * \see derived_has_apply_forces()
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      static inline constexpr auto derived_has_no_apply_forces() noexcept {
        return std::negation<
            decltype(derived_has_apply_forces<TimeType, FirstSystem,
                                              SecondSystem>())>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for overriding torque function in derived
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
       * \tparam FirstSystem Type of first system
       * \tparam SecondSystem Type of second system
       *
       * \metareturns std::bool_constant
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      static inline constexpr auto derived_has_apply_torques() noexcept {
        return ::tt::is_detected<
            detail::Signatures<TimeType, FirstSystem,
                               SecondSystem>::template apply_torques_signature,
            ConcreteConnection>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for overriding apply_torques() in derived
       *
       * \tparam TimeType Type of time
       * \tparam FirstSystem Type of first system
       * \tparam SecondSystem Type of second system
       *
       * \metareturns std::bool_constant
       *
       * \see derived_has_apply_torques()
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      static inline constexpr auto derived_has_no_apply_torques() noexcept {
        return std::negation<
            decltype(derived_has_apply_torques<TimeType, FirstSystem,
                                               SecondSystem>())>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for overriding deferred force in derived
       *
       * \details
       * If the derived class has no apply_forces() member function with a
       * deferred signature, meaning that there is no need to apply forces,
       * we can simply skip evaluation rather than spending time iterating
       * over the indices. This check for skipping can happen in compile-time
       * and is facilitated by this function
       *
       * \note
       * Also works for overloaded functions
       *
       * \tparam TimeType Type of time
       * \tparam FirstSystem Type of first system
       * \tparam SecondSystem Type of second system
       *
       * \metareturns ::std::bool_constant
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      static inline constexpr auto
      derived_has_deferred_apply_forces() noexcept {
        return ::tt::is_detected<
            detail::Signatures<TimeType, FirstSystem, SecondSystem>::
                template deferred_apply_forces_signature,
            ConcreteConnection>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for no overriding deferred force in derived
       *
       * \tparam TimeType Type of time
       * \tparam FirstSystem Type of first system
       * \tparam SecondSystem Type of second system
       *
       * \metareturns std::bool_constant
       *
       * \see derived_has_deferred_apply_forces()
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      static inline constexpr auto
      derived_has_no_deferred_apply_forces() noexcept {
        return std::negation<
            decltype(derived_has_deferred_apply_forces<TimeType, FirstSystem,
                                                       SecondSystem>())>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for overriding deferred torque in derived
       *
       * \details
       * If the derived class has no apply_torques() member function, with a
       * deferred signature, meaning that there is no need to apply torques, we
       * can simply skip evaluation rather than spending time iterating over the
       * indices. This check for skipping can happen in compile-time and is
       * facilitated by this function
       *
       * \note
       * Also works for overloaded functions
       *
       * \tparam TimeType Type of time
       * \tparam FirstSystem Type of first system
       * \tparam SecondSystem Type of second system
       *
       * \metareturns std::bool_constant
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      static inline constexpr auto
      derived_has_deferred_apply_torques() noexcept {
        return ::tt::is_detected<
            detail::Signatures<TimeType, FirstSystem, SecondSystem>::
                template deferred_apply_torques_signature,
            ConcreteConnection>{};
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Compile-time check for no overriding deferred torque in derived
       *
       * \tparam TimeType Type of time
       * \tparam FirstSystem Type of first system
       * \tparam SecondSystem Type of second system
       *
       * \metareturns std::bool_constant
       *
       * \see derived_has_deferred_apply_torques()
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      static inline constexpr auto
      derived_has_no_deferred_apply_torques() noexcept {
        return std::negation<
            decltype(derived_has_deferred_apply_torques<TimeType, FirstSystem,
                                                        SecondSystem>())>{};
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
      virtual ~Connection() = default;
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
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /* time */, FirstSystem& /* first_system*/,
                        std::size_t /* first_index */,
                        SecondSystem& /* second_system */,
                        std::size_t /* second_index */) const;
      //************************************************************************

      //************************************************************************
      /*!\brief The apply_forces() to be overridden by the user, in case the
       * indices are deferred to the user
       *
       * \return void
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_forces(TimeType /* time */, FirstSystem& /* first_system*/,
                        SecondSystem& /* second_system */) const;
      //************************************************************************

      //************************************************************************
      /*!\brief The apply_torques() to be overriden by the user
       *
       * \return void
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType /* time */, FirstSystem& /* first_system*/,
                         std::size_t /* first_index */,
                         SecondSystem& /* second_system */,
                         std::size_t /* second_index */) const;
      //************************************************************************

      //************************************************************************
      /*!\brief The apply_torques() to be overridden by the user, in case the
       * indices are deferred to the user
       *
       * \return void
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem>
      void apply_torques(TimeType /* time */, FirstSystem& /* first_system*/,
                         SecondSystem& /* second_system */) const;
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Connection functions**************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\name Connection functions */
      //@{

      /*!\name Force apply functions */
      //@{

      //************************************************************************
      /*!\brief Apply forces
       *
       * \details
       * Overloaded variant for non-deferred types
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem,
                Requires<derived_has_apply_forces<TimeType, FirstSystem,
                                                  SecondSystem>()> = nullptr>
      auto apply_forces_internal(TimeType t, FirstSystem& first_sys,
                                 std::size_t first_index,
                                 SecondSystem& second_sys,
                                 std::size_t second_index) const -> void {
        self().apply_forces(t, first_sys, first_index, second_sys,
                            second_index);
      }
      //************************************************************************

     private:
      //************************************************************************
      /*!\brief Apply forces alternated
       *
       * \details
       * Overloaded variant for non-deferred types. In case a simple
       * apply_forces() with <FirstSystem, SecondSystem> does not work, we check
       * if the Derived class supports a <SecondSystem, FirstSystem> member and
       * call that indeed, with the help of this function
       */
      template <typename TimeType, typename SecondSystem, typename FirstSystem,
                Requires<derived_has_apply_forces<TimeType, SecondSystem,
                                                  FirstSystem>()> = nullptr>
      inline constexpr auto apply_forces_alternated(
          TimeType t, SecondSystem& second_sys, std::size_t second_index,
          FirstSystem& first_sys, std::size_t first_index) const -> void {
        apply_forces_internal(t, second_sys, second_index, first_sys,
                              first_index);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply forces alternated
       *
       * \details
       * Overloaded variant for non-deferred types. In case a simple
       * apply_forces() with <FirstSystem, SecondSystem> does not work, we check
       * if the Derived class supports a <SecondSystem, FirstSystem> member and
       * call that indeed, with the help of this function.
       *
       * This specialization is when both <FirstSystem, SecondSystem> and
       * <SecondSystem, FirstSystem> members are not present, in which case we
       * assume a no-operation, but present a warning to the user
       */
      template <typename TimeType, typename SecondSystem, typename FirstSystem,
                Requires<derived_has_no_apply_forces<TimeType, SecondSystem,
                                                     FirstSystem>()> = nullptr>
      inline constexpr auto apply_forces_alternated(
          TimeType /*t*/, SecondSystem& /*second_sys*/,
          std::size_t /*xxx_index*/, FirstSystem& /*first_sys*/,
          std::size_t /*first_index*/) const noexcept -> void {
        using Feedback IGNORE_UNUSED =
            decltype(detail::warn_on_finding_no_suitable_force_method<
                     ConcreteConnection, FirstSystem, SecondSystem>{});
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Apply forces
       *
       * \details
       * Overloaded variant for non-deferred types
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem,
                Requires<derived_has_no_apply_forces<TimeType, FirstSystem,
                                                     SecondSystem>()> = nullptr>
      inline constexpr auto apply_forces_internal(
          TimeType t, FirstSystem& first_sys, std::size_t first_index,
          SecondSystem& second_sys, std::size_t second_index) const -> void {
        apply_forces_alternated(t, second_sys, second_index, first_sys,
                                first_index);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply forces
       *
       * \details
       * Overloaded variant for deferred types
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem,
                Requires<derived_has_deferred_apply_forces<
                    TimeType, FirstSystem, SecondSystem>()> = nullptr>
      auto apply_forces_internal(TimeType t, FirstSystem& first_sys,
                                 SecondSystem& second_sys) const -> void {
        self().apply_forces(t, first_sys, second_sys);
      }
      //************************************************************************

     private:
      //************************************************************************
      /*!\brief Apply forces alternated
       *
       * \details
       * Overloaded variant for non-deferred types. In case a simple
       * apply_forces() with <FirstSystem, SecondSystem> does not work, we check
       * if the Derived class supports a <SecondSystem, FirstSystem> member and
       * call that indeed, with the help of this function
       */
      template <typename TimeType, typename SecondSystem, typename FirstSystem,
                Requires<derived_has_deferred_apply_forces<
                    TimeType, SecondSystem, FirstSystem>()> = nullptr>
      inline constexpr auto apply_forces_alternated(
          TimeType t, SecondSystem& second_sys, FirstSystem& first_sys) const
          -> void {
        apply_forces_internal(t, second_sys, first_sys);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply forces alternated
       *
       * \details
       * Overloaded variant for non-deferred types. In case a simple
       * apply_forces() with <FirstSystem, SecondSystem> does not work, we check
       * if the Derived class supports a <SecondSystem, FirstSystem> member and
       * call that indeed, with the help of this function.
       *
       * This specialization is when both <FirstSystem, SecondSystem> and
       * <SecondSystem, FirstSystem> members are not present, in which case we
       * assume a no-operation.
       */
      template <typename TimeType, typename SecondSystem, typename FirstSystem,
                Requires<derived_has_no_deferred_apply_forces<
                    TimeType, SecondSystem, FirstSystem>()> = nullptr>
      inline constexpr auto apply_forces_alternated(
          TimeType /*t*/, SecondSystem& /*second_sys*/,
          FirstSystem& /*first_sys*/) const noexcept -> void {
        using Feedback IGNORE_UNUSED =
            decltype(detail::warn_on_finding_no_suitable_force_method<
                     ConcreteConnection, FirstSystem, SecondSystem>{});
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Apply forces
       *
       * \details
       * Overloaded variant for non-deferred types
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem,
                Requires<derived_has_no_deferred_apply_forces<
                    TimeType, FirstSystem, SecondSystem>()> = nullptr>
      inline constexpr auto apply_forces_internal(
          TimeType t, FirstSystem& first_sys, SecondSystem& second_sys) const
          -> void {
        apply_forces_alternated(t, second_sys, first_sys);
      }
      //************************************************************************

      //@}
      //************************************************************************

      /*!\name Torque apply functions */
      //@{

      //************************************************************************
      /*!\brief Apply torques
       *
       * \details
       * Overloaded variant for non-deferred types
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem,
                Requires<derived_has_apply_torques<TimeType, FirstSystem,
                                                   SecondSystem>()> = nullptr>
      auto apply_torques_internal(TimeType t, FirstSystem& first_sys,
                                  std::size_t first_index,
                                  SecondSystem& second_sys,
                                  std::size_t second_index) const -> void {
        self().apply_torques(t, first_sys, first_index, second_sys,
                             second_index);
      }
      //************************************************************************

     private:
      //************************************************************************
      /*!\brief Apply torques alternated
       *
       * \details
       * Overloaded variant for non-deferred types. In case a simple
       * apply_torques() with <FirstSystem, SecondSystem> does not work, we
       * check if the Derived class supports a <SecondSystem, FirstSystem>
       * member and call that indeed, with the help of this function
       */
      template <typename TimeType, typename SecondSystem, typename FirstSystem,
                Requires<derived_has_apply_torques<TimeType, SecondSystem,
                                                   FirstSystem>()> = nullptr>
      inline constexpr auto apply_torques_alternated(
          TimeType t, SecondSystem& second_sys, std::size_t second_index,
          FirstSystem& first_sys, std::size_t first_index) const -> void {
        apply_torques_internal(t, second_sys, second_index, first_sys,
                               first_index);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply torques alternated
       *
       * \details
       * Overloaded variant for non-deferred types. In case a simple
       * apply_torques() with <FirstSystem, SecondSystem> does not work, we
       * check if the Derived class supports a <SecondSystem, FirstSystem>
       * member and call that indeed, with the help of this function.
       *
       * This specialization is when both <FirstSystem, SecondSystem> and
       * <SecondSystem, FirstSystem> members are not present, in which case we
       * assume a no-operation.
       */
      template <typename TimeType, typename SecondSystem, typename FirstSystem,
                Requires<derived_has_no_apply_torques<TimeType, SecondSystem,
                                                      FirstSystem>()> = nullptr>
      inline constexpr auto apply_torques_alternated(
          TimeType /*t*/, SecondSystem& /*second_sys*/,
          std::size_t /*xxx_index*/, FirstSystem& /*first_sys*/,
          std::size_t /*first_index*/) const noexcept -> void {
        using Feedback IGNORE_UNUSED =
            decltype(detail::warn_on_finding_no_suitable_torque_method<
                     ConcreteConnection, FirstSystem, SecondSystem>{});
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Apply torques
       *
       * \details
       * Overloaded variant for non-deferred types
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem,
                Requires<derived_has_no_apply_torques<
                    TimeType, FirstSystem, SecondSystem>()> = nullptr>
      inline constexpr auto apply_torques_internal(
          TimeType t, FirstSystem& first_sys, std::size_t first_index,
          SecondSystem& second_sys, std::size_t second_index) const -> void {
        apply_torques_alternated(t, second_sys, second_index, first_sys,
                                 first_index);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply torques
       *
       * \details
       * Overloaded variant for deferred types
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem,
                Requires<derived_has_deferred_apply_torques<
                    TimeType, FirstSystem, SecondSystem>()> = nullptr>
      auto apply_torques_internal(TimeType t, FirstSystem& first_sys,
                                  SecondSystem& second_sys) const -> void {
        self().apply_torques(t, first_sys, second_sys);
      }
      //************************************************************************

     private:
      //************************************************************************
      /*!\brief Apply torques alternated
       *
       * \details
       * Overloaded variant for non-deferred types. In case a simple
       * apply_torques() with <FirstSystem, SecondSystem> does not work, we
       * check if the Derived class supports a <SecondSystem, FirstSystem>
       * member and call that indeed, with the help of this function
       */
      template <typename TimeType, typename SecondSystem, typename FirstSystem,
                Requires<derived_has_deferred_apply_torques<
                    TimeType, SecondSystem, FirstSystem>()> = nullptr>
      inline constexpr auto apply_torques_alternated(
          TimeType t, SecondSystem& second_sys, FirstSystem& first_sys) const
          -> void {
        apply_torques_internal(t, second_sys, first_sys);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply torques alternated
       *
       * \details
       * Overloaded variant for non-deferred types. In case a simple
       * apply_torques() with <FirstSystem, SecondSystem> does not work, we
       * check if the Derived class supports a <SecondSystem, FirstSystem>
       * member and call that indeed, with the help of this function.
       *
       * This specialization is when both <FirstSystem, SecondSystem> and
       * <SecondSystem, FirstSystem> members are not present, in which case we
       * assume a no-operation.
       */
      template <typename TimeType, typename SecondSystem, typename FirstSystem,
                Requires<derived_has_no_deferred_apply_torques<
                    TimeType, SecondSystem, FirstSystem>()> = nullptr>
      inline constexpr auto apply_torques_alternated(
          TimeType /*t*/, SecondSystem& /*second_sys*/,
          FirstSystem& /*first_sys*/) const noexcept -> void {
        using Feedback IGNORE_UNUSED =
            decltype(detail::warn_on_finding_no_suitable_torque_method<
                     ConcreteConnection, FirstSystem, SecondSystem>{});
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Apply torques
       *
       * \details
       * Overloaded variant for deferred types
       */
      template <typename TimeType, typename FirstSystem, typename SecondSystem,
                Requires<derived_has_no_deferred_apply_torques<
                    TimeType, FirstSystem, SecondSystem>()> = nullptr>
      auto apply_torques_internal(TimeType t, FirstSystem& first_sys,
                                  SecondSystem& second_sys) const -> void {
        apply_torques_alternated(t, second_sys, first_sys);
      }
      //************************************************************************

      //@}
      //************************************************************************

      //@}
      /*! \endcond */
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace connections

}  // namespace elastica
