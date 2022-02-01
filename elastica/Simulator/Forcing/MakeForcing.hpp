//==============================================================================
/*!
//  \file
//  \brief Defines a factory for making \elastica Forcing objects
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
#include "Simulator/Forcing/Types.hpp"
///
#include "Simulator/Forcing/Forcing.hpp"
#include "Utilities/MakeNamedFunctor.hpp"
#include "Utilities/NamedType.hpp"
#include "Utilities/Overloader.hpp"
#include "Utilities/Registration.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace elastica {

  namespace forcing {

    namespace detail {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declaration of Generic forcing type
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename ForceFunc, typename TorqueFunc, typename Registrars>
      class GenericForcing;
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      namespace Registrars {
        //! Registration type for GenericForcing
        template <typename ForceFunc, typename TorqueFunc>
        using GenericForcing =
            ::Registration::Registrar<GenericForcing, ForceFunc, TorqueFunc>;
      }  // namespace Registrars
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Forcing taking arbitrary callables within \elastica
       * \ingroup forcing
       *
       * \details
       * For rapidly prototyping forcings within \elastica, we utilize a
       * GenericForcing instead of creating custom forcing classes which
       * require a lot of boiler-plate code. The GenericForcing takes two
       * callables (differentiated appropriately into a force and a torque
       * function see below), with specific prototypes, and constructs a
       * standard \elastica forcing with those functions. Usually, these
       * callables are C++ lambda expressions. Rather than using this class
       * directly, users are encouraged to use the make_forcing() free function,
       * which has a more intuitive syntax, and returns a appropriate
       * GenericForcing, respecting arcane C++ rules.
       *
       * \tparam ForceFunc Type of force functor
       * \tparam TorqueFunc Type of torque functor
       * \tparam Registrars Registration helpers
       *
       * \see make_forcing(), make_force(), make_torque()
       */
      template <typename ForceFunc, typename TorqueFunc,
                typename Registrars = tmpl::list<
                    Registrars::GenericForcing<ForceFunc, TorqueFunc>>>
      class GenericForcing final
          : public Forcing<Registrars,
                           GenericForcing<ForceFunc, TorqueFunc, Registrars>> {
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
        GenericForcing(ForceFunc const& v, TorqueFunc const& r)
            : force_(v), torque_(r) {}
        //**********************************************************************

        //**********************************************************************
        /*!\brief Constructor for move-able functors
         */
        GenericForcing(ForceFunc&& v, TorqueFunc&& r) noexcept
            : force_(std::move(v)), torque_(std::move(r)) {}
        //**********************************************************************

        //**********************************************************************
        /*!\brief The move constructor.
         */
        GenericForcing(GenericForcing&& other) noexcept
            : force_(std::move(other.force_)),
              torque_(std::move(other.torque_)) {}
        //@}
        //**********************************************************************

       public:
        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        //**********************************************************************
        /*!\brief destructor
         */
        ~GenericForcing() final = default;
        //@}
        //**********************************************************************

        //**Forcing functions************************************************
        /*!\name Forcing functions */
        //@{

        //**********************************************************************
        /*!\brief Apply forces
         *
         * \see make_force()
         */
        template <typename TimeType, typename System>
        inline auto apply_forces(TimeType t, System& sys, int index) const
            -> void {
          force_(t, sys, index);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply torques
         *
         * \see make_torque()
         */
        template <typename TimeType, typename System>
        inline auto apply_torques(TimeType t, System& sys, int index) const
            -> void {
          torque_(t, sys, index);
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

    namespace make_forcing_detail {
      // only if the current type is a rvalue reference = !lvalue_reference, and
      // is not copy constructible you can consider moving into generic
      // forcing. The non-copyable restriction arises because the factory
      // returned from make_forcing() can be used multiple times.
      // here we use deMoivre's theorem as !(A or B) = !A and !B
      template <typename T>
      constexpr bool should_be_moved = cpp17::negation<cpp17::disjunction<
          std::is_lvalue_reference<T>, std::is_copy_constructible<T>>>::value;
    }  // namespace make_forcing_detail

    namespace make_forcing_detail {
      // Note this cannot have a std::unique_ptr variant because by definition
      // this is going to be a compile-time class so maybe a more descriptive
      // name for the function is better
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Makes a GenericForcing from two functors
       * \ingroup forcing
       *
       * \details
       * This is the SFINAE specialization for two copyable forcing
       *
       * \param f Forces functor
       * \param g Torques functor
       * \return GenericForcing initialized with f and g
       *
       * \see GenericForcing
       */
      template <typename F, typename G,
                Requires<!make_forcing_detail::should_be_moved<F>> = nullptr,
                Requires<!make_forcing_detail::should_be_moved<G>> = nullptr>
      auto make_forcing(F&& f, G&& g) {
        using dF = std::decay_t<F>;
        using dG = std::decay_t<G>;
        using ReturnType = detail::GenericForcing<dF, dG>;
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
      /*!\brief make_forcing() specialization for two moveable functors
       * \ingroup forcing
       *
       * \param f Forces functor
       * \param g Torques functor
       * \return GenericForcing initialized with f and g
       *
       * \see GenericForcing
       */
      template <typename F, typename G,
                Requires<make_forcing_detail::should_be_moved<F>> = nullptr,
                Requires<make_forcing_detail::should_be_moved<G>> = nullptr>
      auto make_forcing(F&& f, G&& g) noexcept {
        using ReturnType =
            detail::GenericForcing<std::decay_t<F>, std::decay_t<G>>;
        struct {
          F f;
          G g;
        } capture{std::forward<F>(f), std::forward<G>(g)};
        return [already_called = false,
                capture = std::move(capture)]() mutable -> ReturnType {
          if (already_called)
            throw std::runtime_error(
                "Cannot call make_forcing factory more than "
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
      /*!\brief make_forcing() specialization for f moveable
       * \ingroup forcing
       *
       * \param f Forces functor
       * \param g Torques functor
       * \return GenericForcing initialized with f and g
       *
       * \see GenericForcing
       */
      template <typename F, typename G,
                Requires<make_forcing_detail::should_be_moved<F>> = nullptr,
                Requires<!make_forcing_detail::should_be_moved<G>> = nullptr>
      auto make_forcing(F&& f, G&& g) noexcept {
        using ReturnType =
            detail::GenericForcing<std::decay_t<F>, std::decay_t<G>>;
        struct {
          F f;
          G g;
        } capture{std::forward<F>(f), std::forward<G>(g)};
        return [already_called = false,
                capture = std::move(capture)]() mutable -> ReturnType {
          if (already_called)
            throw std::runtime_error(
                "Cannot call make_forcing factory more than once since "
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
      /*!\brief make_forcing() specialization for g moveable
       * \ingroup forcing
       *
       * \param f Forces functor
       * \param g Torques functor
       * \return GenericForcing initialized with f and g
       *
       * \see GenericForcing
       */
      template <typename F, typename G,
                Requires<!make_forcing_detail::should_be_moved<F>> = nullptr,
                Requires<make_forcing_detail::should_be_moved<G>> = nullptr>
      auto make_forcing(F&& f, G&& g) noexcept {
        using ReturnType =
            detail::GenericForcing<std::decay_t<F>, std::decay_t<G>>;
        struct {
          F f;
          G g;
        } capture{std::forward<F>(f), std::forward<G>(g)};
        return [already_called = false,
                capture = std::move(capture)]() mutable -> ReturnType {
          if (already_called)
            throw std::runtime_error(
                "Cannot call make_forcing factory more than once since "
                "the second functor is non-copyable and has been moved!");
          else {
            already_called = true;
            return ReturnType(std::move(capture.f), std::move(capture.g));
          }
        };
      }
      /*! \endcond */
      //************************************************************************

    }  // namespace make_forcing_detail

    //**Type definitions********************************************************
    /*! \cond ELASTICA_INTERNAL */
    //! Strongly type forces
    template <typename Func>
    using Forces = named_type::NamedType<Func, struct ForcingParameter,
                                         named_type::Gettable>;

    //! Strongly typed torques
    template <typename Func>
    using Torques = named_type::NamedType<Func, struct TorqueParameter,
                                          named_type::Gettable>;
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes a force from a set of overloaded functors
     * \ingroup forcing
     *
     * \details
     * Marks the input functor f, others... as functions that can apply forces
     * within the context of an \elastica forcing.
     *
     * \example
     * The following shows a typical use of a force with a generic
     * lambda. Note that the lambda function needs to confirm to the expected
     * signature of a Forcing function!
     * \snippet Test_MakeForcing.cpp make_force_example
     *
     * However, you can also pass in several lambda functions, specialized for
     * different systems of your choosing:
     * \snippet Test_MakeForcing.cpp make_force_overloaded_example
     *
     * \param f Input function that acts as a force
     * \param funcs Input functions that act as a force, for overloading
     *
     * \see make_forcing()
     */
    template <typename F, typename... Funcs>
    constexpr inline decltype(auto)
    make_force(F&& f, Funcs&&... funcs) noexcept(
        noexcept(named_type::make_named_functor<Forces>(
            std::forward<F>(f), std::forward<Funcs>(funcs)...))) {
      return named_type::make_named_functor<Forces>(
          std::forward<F>(f), std::forward<Funcs>(funcs)...);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes a torque from a set of overloaded functors
     * \ingroup forcing
     *
     * \details
     * Marks the input functor f, others... as functions that can apply torques
     * within the context of an \elastica forcing.
     *
     * \example
     * The following shows a typical use of a torque with a generic
     * lambda. Note that the lambda function needs to confirm to the expected
     * signature of a Forcing function!
     * \snippet Test_MakeForcing.cpp make_torque_example
     *
     * However, you can also pass in several lambda functions, specialized for
     * different systems of your choosing:
     * \snippet Test_MakeForcing.cpp make_torque_overloaded_example
     *
     * \param f Input function that acts as a torque
     * \param funcs Input functions that act as a torque, for overloading
     *
     * \see make_forcing()
     */
    template <typename F, typename... Funcs>
    constexpr inline decltype(auto)
    make_torque(F&& f, Funcs&&... funcs) noexcept(
        noexcept(named_type::make_named_functor<Torques>(
            std::forward<F>(f), std::forward<Funcs>(funcs)...))) {
      return named_type::make_named_functor<Torques>(
          std::forward<F>(f), std::forward<Funcs>(funcs)...);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes an arbitrary \elastica forcing in an idiomatic way
     * \ingroup forcing
     *
     * \details
     * For rapidly prototyping forcing within \elastica, we have this
     * alternate mechanism instead of creating custom Forcing classes which
     * require a lot of boiler-plate code. The make_forcing() function takes
     * two callables (differentiated appropriately into a force and a torque
     * function, see below), with specific prototypes, and constructs a standard
     * \elastica Forcing with those functions. Usually, but not necessarily,
     * these callables are C++ lambda expressions. The following example is an
     * idiomatic way of creating a new Forcing
     *
     * \example
     * \snippet Test_MakeForcing.cpp make_forcing_example
     *
     * We draw attention to the semantics and readability provided by using the
     * concomitant functions make_force(), make_torque().
     * Note that the order of make_force() and make_torque() do not matter.
     * Other use cases are possible, and we refer you to peruse
     * Test_MakeForcing.cpp in the tests folders for patterns. Interestingly,
     * one can also make parametrized forcing (i.e. forcing with varying
     * parameters), in the following fashion
     *
     * \snippet Test_MakeForcing.cpp make_parametrized_forcing_example
     *
     * The above two should cover >90% of all use cases. Having introduced this
     * function, users are encouraged to write custom \elastica Forcing classes
     * only if
     *
     * - this does not satisfy their more complex use case
     * - runtime parameter support (loading from file, archiving it) is needed
     * - you want to record, document and reuse the custom Forcing class
     *
     * \param forces A functor marked with make_force()
     * \param torques A functor marked with make_torque()
     * \return GenericForcing initialized with make_force() and
     * make_torque()
     *
     * \see make_force(), make_torque()
     */
    template <typename F, typename G>
    auto make_forcing(Forces<F>&& forces, Torques<G>&& torques) noexcept(
        noexcept(make_forcing_detail::make_forcing(std::move(forces.get()),
                                                   std::move(torques.get())))) {
      return make_forcing_detail::make_forcing(std::move(forces.get()),
                                               std::move(torques.get()));
    }
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of make_forcing for switched arguments
     * \ingroup forcing
     */
    template <typename F, typename G>
    auto make_forcing(Torques<G>&& torques, Forces<F>&& forces) noexcept(
        noexcept(make_forcing_detail::make_forcing(std::move(forces.get()),
                                                   std::move(torques.get())))) {
      return make_forcing_detail::make_forcing(std::move(forces.get()),
                                               std::move(torques.get()));
    }
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes an arbitrary \elastica forcing in an idiomatic way
     * \ingroup forcing
     *
     * \details
     * This overload make_forcing() takes in any number of callables directly
     * These callables need to apply BOTH forces and torques on the system---the
     * user is responsible to ensure that both forces and torques are applied.
     * Except for this, all other features are retained, so see the
     * documentation of the other make_forcing() overload.
     *
     * \example
     * \snippet Test_MakeForcing.cpp make_unified_example
     *
     * You can also overload it so that the right function is called based on
     * the type of the system
     *
     * \snippet Test_MakeForcing.cpp make_unified_overloading_eg
     *
     * \param forces_and_torques A functor that applies forces and torques
     * \param other_forces_and_torques Other force & torque functors
     * \return GenericForcing initialized with make_force() and
     * make_torque()
     */
    template <typename F, typename... Funcs>
    auto make_forcing(F forces_and_torques, Funcs... other_forces_and_torques) {
      // don't worry about noexcept, it will be refactored later anyway
      return make_forcing_detail::make_forcing(
          make_overloader(std::move(forces_and_torques),
                          std::move(other_forces_and_torques)...),
          // use an auto ref to not create a copy
          [](auto&...) noexcept {});
    }
    //**************************************************************************

  }  // namespace forcing

}  // namespace elastica
