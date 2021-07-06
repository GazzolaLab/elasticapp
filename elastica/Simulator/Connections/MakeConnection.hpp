//==============================================================================
/*!
//  \file
//  \brief Defines a factory for making \elastica Connection objects
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

#include <stdexcept>
///
#include "Simulator/Connections/Types.hpp"
///
#include "Simulator/Connections/Connection.hpp"
#include "Utilities/MakeNamedFunctor.hpp"
#include "Utilities/NamedType.hpp"
#include "Utilities/Registration.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace elastica {

  namespace connections {

    namespace detail {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declaration of Generic connections type
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename ForceFunc, typename TorqueFunc, typename Registrars>
      class GenericConnection;
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      namespace Registrars {
        //! Registration type for GenericConnection
        template <typename ForceFunc, typename TorqueFunc>
        using GenericConnection =
            ::Registration::Registrar<GenericConnection, ForceFunc, TorqueFunc>;
      }  // namespace Registrars
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Connection taking arbitrary callables within \elastica
       * \ingroup connections
       *
       * \details
       * For rapidly prototyping connections within \elastica, we utilize a
       * GenericConnection instead of creating custom connections classes which
       * require a lot of boiler-plate code. The GenericConnection takes two
       * callables (differentiated appropriately into a force and a torque
       * function see below), with specific prototypes, and constructs a
       * standard \elastica connection with those functions. Usually, these
       * callables are C++ lambda expressions. Rather than using this class
       * directly, users are encouraged to use the make_connection() free
       * function, which has a more intuitive syntax, and returns a appropriate
       * GenericConnection, respecting arcane C++ rules.
       *
       * \tparam ForceFunc Type of force functor
       * \tparam TorqueFunc Type of torque functor
       * \tparam Registrars Registration helpers
       *
       * \see make_connection(), make_force(), make_torque()
       */
      template <typename ForceFunc, typename TorqueFunc,
                typename Registrars = tmpl::list<
                    Registrars::GenericConnection<ForceFunc, TorqueFunc>>>
      class GenericConnection final
          : public Connection<
                Registrars,
                GenericConnection<ForceFunc, TorqueFunc, Registrars>> {
       public:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief SFINAE Constructor for copyable functors
         */
        template <
            typename F = ForceFunc, typename G = TorqueFunc,
            Requires<cpp17::conjunction_v<std::is_copy_constructible<F>,
                                          std::is_copy_constructible<G>>> =
                nullptr>
        GenericConnection(ForceFunc const& v, TorqueFunc const& r)
            : force_(v), torque_(r){}
        //**********************************************************************

        //**********************************************************************
        /*!\brief Constructor for move-able functors
         */
        GenericConnection(ForceFunc&& v, TorqueFunc&& r) noexcept
            : force_(std::move(v)), torque_(std::move(r)){};
        //**********************************************************************

        //**********************************************************************
        /*!\brief The move constructor.
         */
        GenericConnection(GenericConnection&& other) noexcept
            : force_(std::move(other.force_)),
              torque_(std::move(other.torque_)) {}
        //@}
        //**********************************************************************

       protected:
        //**Metafunctions*******************************************************
        /*!\name Meta-programming functions
           Helper functions for metaprogramming generic constraints*/
        //@{

        //**********************************************************************
        /*!\brief Compile-time check for rightly ordered functions
         *
         * \details
         * If the template functor `Func` has its `operator()` overloaded for
         * <Second, First> rather than <First, Second>, we need to switch
         * arguments. This check for swapping arguments can happen in
         * compile-time and is facilitated by this function.
         *
         * \note
         * Also works for overloaded functions
         *
         * \tparam Func One of ForceFunc or TorqueFunc
         * \tparam TimeType Type of time
         * \tparam FirstSystem Type of first system
         * \tparam SecondSystem Type of second system
         *
         * \metareturns ::cpp17::bool_constant
         *
         * \see force_is_rightly_ordered(), torque_is_rightly_ordered()
         */
        template <typename Func, typename TimeType, typename FirstSystem,
                  typename SecondSystem>
        static inline constexpr auto is_rightly_ordered() noexcept {
          return ::tt::is_detected<
              detail::Signatures<TimeType, FirstSystem,
                                 SecondSystem>::template nonwrapped_signature,
              Func>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a rightly ordered force function
         *
         * \see is_rightly_ordered()
         */
        template <typename TimeType, typename FirstSystem,
                  typename SecondSystem>
        static inline constexpr auto force_is_rightly_ordered() noexcept {
          return is_rightly_ordered<ForceFunc, TimeType, FirstSystem,
                                    SecondSystem>();
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for an incorrectly ordered force function
         *
         * \see force_is_rightly_ordered()
         */
        template <typename TimeType, typename FirstSystem,
                  typename SecondSystem>
        static inline constexpr auto force_is_not_rightly_ordered() noexcept {
          return cpp17::negation<
              decltype(force_is_rightly_ordered<TimeType, FirstSystem,
                                                SecondSystem>())>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a rightly ordered torque functions
         *
         * \see is_rightly_ordered()
         */
        template <typename TimeType, typename FirstSystem,
                  typename SecondSystem>
        static inline constexpr auto torque_is_rightly_ordered() noexcept {
          return is_rightly_ordered<TorqueFunc, TimeType, FirstSystem,
                                    SecondSystem>();
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for an incorrectly ordered torque function
         *
         * \see torque_is_rightly_ordered()
         */
        template <typename TimeType, typename FirstSystem,
                  typename SecondSystem>
        static inline constexpr auto torque_is_not_rightly_ordered() noexcept {
          return cpp17::negation<
              decltype(torque_is_rightly_ordered<TimeType, FirstSystem,
                                                 SecondSystem>())>{};
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       public:
        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        /*!\brief destructor
         */
        ~GenericConnection() final = default;
        //@}
        //**********************************************************************

        //**Connection functions************************************************
        /*!\name Connection functions */
        //@{

        //**********************************************************************
        /*!\brief Apply forces
         *
         * \see make_force()
         */
        template <typename TimeType, typename FirstSystem,
                  typename SecondSystem,
                  Requires<force_is_rightly_ordered<TimeType, FirstSystem,
                                                    SecondSystem>()> = nullptr>
        inline auto apply_forces(TimeType t, FirstSystem& first_sys,
                                 int first_idx, SecondSystem& second_sys,
                                 int second_idx) const -> void {
          force_(t, first_sys, first_idx, second_sys, second_idx);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply forces
         *
         * \see make_force()
         */
        template <typename TimeType, typename FirstSystem,
                  typename SecondSystem,
                  Requires<force_is_not_rightly_ordered<
                      TimeType, FirstSystem, SecondSystem>()> = nullptr>
        inline auto apply_forces(TimeType t, FirstSystem& first_sys,
                                 int first_idx, SecondSystem& second_sys,
                                 int second_idx) const -> void {
          force_(t, second_sys, second_idx, first_sys, first_idx);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply torques
         *
         * \see make_torque()
         */
        template <typename TimeType, typename FirstSystem,
                  typename SecondSystem,
                  Requires<torque_is_rightly_ordered<TimeType, FirstSystem,
                                                     SecondSystem>()> = nullptr>
        inline auto apply_torques(TimeType t, FirstSystem& first_sys,
                                  int first_idx, SecondSystem& second_sys,
                                  int second_idx) const -> void {
          torque_(t, first_sys, first_idx, second_sys, second_idx);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply torques
         *
         * \see make_torque()
         */
        template <typename TimeType, typename FirstSystem,
                  typename SecondSystem,
                  Requires<torque_is_not_rightly_ordered<
                      TimeType, FirstSystem, SecondSystem>()> = nullptr>
        inline auto apply_torques(TimeType t, FirstSystem& first_sys,
                                  int first_idx, SecondSystem& second_sys,
                                  int second_idx) const -> void {
          torque_(t, second_sys, second_idx, first_sys, first_idx);
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! Functor applying forces
        ForceFunc force_;
        //! Functor applying torques
        TorqueFunc torque_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

    namespace make_connection_detail {
      // only if the current type is a rvalue reference = !lvalue_reference, and
      // is not copy constructible you can consider moving into generic
      // connections. The non-copyable restriction arises because the factory
      // returned from make_connection() can be used multiple times.
      // here we use deMoivre's theorem as !(A or B) = !A and !B
      template <typename T>
      constexpr bool should_be_moved = cpp17::negation<cpp17::disjunction<
          std::is_lvalue_reference<T>, std::is_copy_constructible<T>>>::value;
    }  // namespace make_connection_detail

    namespace make_connection_detail {
      // Note this cannot have a std::unique_ptr variant because by definition
      // this is going to be a compile-time class so maybe a more descriptive
      // name for the function is better
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Makes a GenericConnection from two functors
       * \ingroup connections
       *
       * \details
       * This is the SFINAE specialization for two copyable connections
       *
       * \param f Forces functor
       * \param g Torques functor
       * \return GenericConnection initialized with f and g
       *
       * \see GenericConnection
       */
      template <typename F, typename G,
                Requires<!make_connection_detail::should_be_moved<F>> = nullptr,
                Requires<!make_connection_detail::should_be_moved<G>> = nullptr>
      auto make_connection(F&& f, G&& g) {
        using dF = std::decay_t<F>;
        using dG = std::decay_t<G>;
        using ReturnType = detail::GenericConnection<dF, dG>;
        struct {
          F f;
          G g;
        } capture{std::forward<F>(f), std::forward<G>(g)};
        return [capture = std::move(capture)]() noexcept(
                   cpp17::conjunction_v<std::is_nothrow_copy_constructible<dF>,
                                        std::is_nothrow_copy_constructible<dG>>)
                   -> ReturnType { return ReturnType(capture.f, capture.g); };
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief make_connection() specialization for two moveable functors
       * \ingroup connections
       *
       * \param f Forces functor
       * \param g Torques functor
       * \return GenericConnection initialized with f and g
       *
       * \see GenericConnection
       */
      template <typename F, typename G,
                Requires<make_connection_detail::should_be_moved<F>> = nullptr,
                Requires<make_connection_detail::should_be_moved<G>> = nullptr>
      auto make_connection(F&& f, G&& g) noexcept {
        using ReturnType =
            detail::GenericConnection<std::decay_t<F>, std::decay_t<G>>;
        struct {
          F f;
          G g;
        } capture{std::forward<F>(f), std::forward<G>(g)};
        return [already_called = false,
                capture = std::move(capture)]() mutable -> ReturnType {
          if (already_called)
            throw std::runtime_error(
                "Cannot call make_connection factory more than "
                "once "
                "since both the functors are non-copyable and"
                "have been moved already!");
          else {
            already_called = true;
            return ReturnType(std::move(capture.f), std::move(capture.g));
          }
        };
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief make_connection() specialization for f moveable
       * \ingroup connections
       *
       * \param f Forces functor
       * \param g Torques functor
       * \return GenericConnection initialized with f and g
       *
       * \see GenericConnection
       */
      template <typename F, typename G,
                Requires<make_connection_detail::should_be_moved<F>> = nullptr,
                Requires<!make_connection_detail::should_be_moved<G>> = nullptr>
      auto make_connection(F&& f, G&& g) noexcept {
        using ReturnType =
            detail::GenericConnection<std::decay_t<F>, std::decay_t<G>>;
        struct {
          F f;
          G g;
        } capture{std::forward<F>(f), std::forward<G>(g)};
        return [already_called = false,
                capture = std::move(capture)]() mutable -> ReturnType {
          if (already_called)
            throw std::runtime_error(
                "Cannot call make_connection factory more than once since "
                "the first functor is non-copyable and has been moved!");
          else {
            already_called = true;
            return ReturnType(std::move(capture.f), std::move(capture.g));
          }
        };
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief make_connection() specialization for g moveable
       * \ingroup connections
       *
       * \param f Forces functor
       * \param g Torques functor
       * \return GenericConnection initialized with f and g
       *
       * \see GenericConnection
       */
      template <typename F, typename G,
                Requires<!make_connection_detail::should_be_moved<F>> = nullptr,
                Requires<make_connection_detail::should_be_moved<G>> = nullptr>
      auto make_connection(F&& f, G&& g) noexcept {
        using ReturnType =
            detail::GenericConnection<std::decay_t<F>, std::decay_t<G>>;
        struct {
          F f;
          G g;
        } capture{std::forward<F>(f), std::forward<G>(g)};
        return [already_called = false,
                capture = std::move(capture)]() mutable -> ReturnType {
          if (already_called)
            throw std::runtime_error(
                "Cannot call make_connection factory more than once since "
                "the second functor is non-copyable and has been moved!");
          else {
            already_called = true;
            return ReturnType(std::move(capture.f), std::move(capture.g));
          }
        };
      }
      /*! \endcond */
      //************************************************************************

    }  // namespace make_connection_detail

    //**Type definitions********************************************************
    /*! \cond ELASTICA_INTERNAL */
    //! Strongly type forces
    template <typename Func>
    using ConnectionForces =
        named_type::NamedType<Func, struct ConnectionForceParameter,
                              named_type::Gettable>;

    //! Strongly typed torques
    template <typename Func>
    using ConnectionTorques =
        named_type::NamedType<Func, struct ConnectionTorqueParameter,
                              named_type::Gettable>;
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes a force from a set of overloaded functors
     * \ingroup connections
     *
     * \details
     * Marks the input functor f, others... as functions that can apply forces
     * within the context of an \elastica connection.
     *
     * \example
     * The following shows a typical use of a force with a generic
     * lambda. Note that the lambda function needs to confirm to the expected
     * signature of a Connection force function!
     * \snippet Test_MakeConnection.cpp make_force_example
     *
     * However, you can also pass in several lambda functions, specialized for
     * different systems of your choosing:
     * \snippet Test_MakeConnection.cpp make_force_overloaded_example
     *
     * \param f Input function that acts as a force
     * \param funcs Input functions that act as a force, for overloading
     *
     * \see make_connection()
     */
    template <typename F, typename... Funcs>
    constexpr inline decltype(auto)
    make_force(F&& f, Funcs&&... funcs) noexcept(
        noexcept(named_type::make_named_functor<ConnectionForces>(
            std::forward<F>(f), std::forward<Funcs>(funcs)...))) {
      return named_type::make_named_functor<ConnectionForces>(
          std::forward<F>(f), std::forward<Funcs>(funcs)...);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes a torque from a set of overloaded functors
     * \ingroup connections
     *
     * \details
     * Marks the input functor f, others... as functions that can apply torques
     * within the context of an \elastica connection.
     *
     * \example
     * The following shows a typical use of a torque with a generic
     * lambda. Note that the lambda function needs to confirm to the expected
     * signature of a Connection torque function!
     * \snippet Test_MakeConnection.cpp make_torque_example
     *
     * However, you can also pass in several lambda functions, specialized for
     * different systems of your choosing:
     * \snippet Test_MakeConnection.cpp make_torque_overloaded_example
     *
     * \param f Input function that acts as a torque
     * \param funcs Input functions that act as a torque, for overloading
     *
     * \see make_connection()
     */
    template <typename F, typename... Funcs>
    constexpr inline decltype(auto)
    make_torque(F&& f, Funcs&&... funcs) noexcept(
        noexcept(named_type::make_named_functor<ConnectionTorques>(
            std::forward<F>(f), std::forward<Funcs>(funcs)...))) {
      return named_type::make_named_functor<ConnectionTorques>(
          std::forward<F>(f), std::forward<Funcs>(funcs)...);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes an arbitrary \elastica connection in an idiomatic way
     * \ingroup connections
     *
     * \details
     * For rapidly prototyping connections within \elastica, we have this
     * alternate mechanism instead of creating custom Connection classes which
     * require a lot of boiler-plate code. The make_connection() function takes
     * two callables (differentiated appropriately into a force and a torque
     * function, see below), with specific prototypes, and constructs a standard
     * \elastica Connection with those functions. Usually, but not necessarily,
     * these callables are C++ lambda expressions. The following example is an
     * idiomatic way of creating a new Connection
     *
     * \example
     * \snippet Test_MakeConnection.cpp make_connection_example
     *
     * We draw attention to the semantics and readability provided by using the
     * concomitant functions make_torque(), make_force().
     * Note that the order of make_force() and make_torque() do not matter.
     * Other use cases are possible, and we refer you to peruse
     * Test_MakeConnection.cpp in the tests folders for patterns. Interestingly,
     * one can also make parametrized connections (i.e. connections with
     * varying parameters), in the following fashion
     *
     * \snippet Test_MakeConnection.cpp make_parametrized_connection_example
     *
     * The above two should cover >90% of all use cases. Having introduced this
     * function, users are encouraged to write custom \elastica Connection
     * classes only if
     *
     * - this does not satisfy their more complex use case
     * - runtime parameter support (loading from file, archiving it) is needed
     * - you want to record, document and reuse the custom Connection class
     *
     * \param forces A functor marked with make_force()
     * \param torques A functor marked with make_torque()
     * \return GenericConnection initialized with make_force() and
     * make_torque()
     *
     * \see make_force(), make_torque()
     */
    template <typename F, typename G>
    auto make_connection(
        ConnectionForces<F>&& forces,
        ConnectionTorques<G>&&
            torques) noexcept(noexcept(make_connection_detail::
                                           make_connection(
                                               std::move(forces.get()),
                                               std::move(torques.get())))) {
      return make_connection_detail::make_connection(std::move(forces.get()),
                                                     std::move(torques.get()));
    }
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of make_connection for switched arguments
    // \ingroup connections
    */
    template <typename F, typename G>
    auto make_connection(
        ConnectionTorques<G>&& torques,
        ConnectionForces<F>&&
            forces) noexcept(noexcept(make_connection_detail::
                                          make_connection(
                                              std::move(forces.get()),
                                              std::move(torques.get())))) {
      return make_connection_detail::make_connection(std::move(forces.get()),
                                                     std::move(torques.get()));
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace connections

}  // namespace elastica
