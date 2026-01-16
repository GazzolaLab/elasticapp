//==============================================================================
/*!
//  \file
//  \brief Defines a factory for making \elastica Constraint objects
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
#include "Simulator/Constraints/Types.hpp"
///
#include "Simulator/Constraints/Constraint.hpp"
#include "Utilities/MakeNamedFunctor.hpp"
#include "Utilities/NamedType.hpp"
#include "Utilities/Registration.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace constraints {

    namespace detail {

      //////////////////////////////////////////////////////////////////////////
      //
      // Forward declaration of Generic constraint type
      //
      //////////////////////////////////////////////////////////////////////////

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      template <typename ValueConstraintFunc, typename RateConstraintFunc,
                typename Registrars>
      class GenericConstraint;
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      namespace Registrars {
        //! Registration type for GenericConstraint
        template <typename ValueConstraintFunc, typename RateConstraintFunc>
        using GenericConstraint =
            ::Registration::Registrar<GenericConstraint, ValueConstraintFunc,
                                      RateConstraintFunc>;
      }  // namespace Registrars
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Constraint taking arbitrary callables within \elastica
       * \ingroup constraints
       *
       * \details
       * For rapidly prototyping constraints within \elastica, we utilize a
       * GenericConstraint instead of creating custom constraint classes which
       * require a lot of boiler-plate code. The GenericConstraint takes two
       * callables (differentiated appropriately into a value and rate function
       * see below), with specific prototypes, and constructs a standard
       * \elastica constraint with those functions. Usually, these callables are
       * C++ lambda expressions. Rather than using this class directly, users
       * are encouraged to use the make_constraint() free function, which has a
       * more intuitive syntax, and returns a appropriate GenericConstraint,
       * respecting arcane C++ rules.
       *
       * \tparam ValueConstraintFunc Type of value constraint functor
       * \tparam RateConstraintFunc Type of rate constraint functor
       * \tparam Registrars Registration helpers
       *
       * \see make_constraint(), make_rate_constraint(), make_value_constraint()
       */
      template <typename ValueConstraintFunc, typename RateConstraintFunc,
                typename Registrars = tmpl::list<Registrars::GenericConstraint<
                    ValueConstraintFunc, RateConstraintFunc>>>
      class GenericConstraint final
          : public Constraint<
                Registrars, GenericConstraint<ValueConstraintFunc,
                                              RateConstraintFunc, Registrars>> {
       public:
        // purely for convenience
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief Base Constructor
         */
        GenericConstraint(ValueConstraintFunc vc,
                          RateConstraintFunc rc) noexcept
            : value_constraint_(std::move(vc)),
              rate_constraint_(std::move(rc)){};
        //**********************************************************************

        //**********************************************************************
        /*!\brief The move constructor.
         */
        GenericConstraint(GenericConstraint&& other) noexcept
            : value_constraint_(std::move(other.value_constraint_)),
              rate_constraint_(std::move(other.rate_constraint_)) {}
        //@}
        //**********************************************************************

       public:
        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        //**********************************************************************
        /*!\brief destructor
         */
        ~GenericConstraint() final = default;
        //@}
        //**********************************************************************

        //**Constraint functions************************************************
        /*!\name Constraint functions */
        //@{

        //**********************************************************************
        /*!\brief Constrain primitives
         *
         * \see make_value_constraint()
         */
        template <typename TimeType, typename System>
        inline auto constrain_primitives(TimeType t, System& sys,
                                         std::size_t index) const -> void {
          value_constraint_(t, sys, index);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Constrain rates
         *
         * \see make_rate_constraint()
         */
        template <typename TimeType, typename System>
        inline auto constrain_rates(TimeType t, System& sys,
                                    std::size_t index) const -> void {
          rate_constraint_(t, sys, index);
        }
        //**********************************************************************
        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! Functor constraining primitives
        ValueConstraintFunc value_constraint_;
        //! Functor constraining rates
        RateConstraintFunc rate_constraint_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace detail

    namespace make_constraint_detail {
      // only if the current type is a rvalue reference = !lvalue_reference, and
      // is not copy constructible you can consider moving into generic
      // constraint. The non-copyable restriction arises because the factory
      // returned from make_constraint() can be used multiple times
      // !(A or B) = !A and !B
      template <typename T>
      constexpr bool should_be_moved = std::negation<std::disjunction<
          std::is_lvalue_reference<T>, std::is_copy_constructible<T>>>::value;
    }  // namespace make_constraint_detail

    namespace make_constraint_detail {
      // Note this cannot have a std::unique_ptr variant because by definition
      // this is going to be a compile-time class so maybe a more descriptive
      // name for the function is better
      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Makes a GenericConstraint from two functors
       * \ingroup constraints
       *
       * \details
       * This is the SFINAE specialization for two copyable constraints
       *
       * \param f Value constraint functor
       * \param g Rate constraint functor
       * \return GenericConstraint initialized with f and g
       *
       * \see GenericConstraint
       */
      template <typename F, typename G,
                Requires<!make_constraint_detail::should_be_moved<F>> = nullptr,
                Requires<!make_constraint_detail::should_be_moved<G>> = nullptr>
      auto make_constraint(F&& f, G&& g) {
        using dF = std::decay_t<F>;
        using dG = std::decay_t<G>;
        using ReturnType = detail::GenericConstraint<dF, dG>;
        struct {
          F f;
          G g;
        } capture{std::forward<F>(f), std::forward<G>(g)};
        // is not noexcept given that the copy constructor can throw
        return [capture = std::move(capture)]() noexcept(
                   std::conjunction_v<std::is_nothrow_copy_constructible<dF>,
                                        std::is_nothrow_copy_constructible<dG>>)
                   -> ReturnType { return ReturnType(capture.f, capture.g); };
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief make_constraint() specialization for two moveable functors
       * \ingroup constraints
       *
       * \param f Value constraint functor
       * \param g Rate constraint functor
       * \return GenericConstraint initialized with f and g
       *
       * \see GenericConstraint
       */
      template <typename F, typename G,
                Requires<make_constraint_detail::should_be_moved<F>> = nullptr,
                Requires<make_constraint_detail::should_be_moved<G>> = nullptr>
      auto make_constraint(F&& f, G&& g) noexcept {
        using ReturnType =
            detail::GenericConstraint<std::decay_t<F>, std::decay_t<G>>;
        struct {
          F f;
          G g;
        } capture{std::forward<F>(f), std::forward<G>(g)};
        return [already_called = false,
                capture = std::move(capture)]() mutable -> ReturnType {
          if (already_called)
            throw std::runtime_error(
                "Cannot call make_constraint factory more than "
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
      /*!\brief make_constraint() specialization for f moveable
       * \ingroup constraints
       *
       * \param f Value constraint functor
       * \param g Rate constraint functor
       * \return GenericConstraint initialized with f and g
       *
       * \see GenericConstraint
       */
      template <typename F, typename G,
                Requires<make_constraint_detail::should_be_moved<F>> = nullptr,
                Requires<!make_constraint_detail::should_be_moved<G>> = nullptr>
      auto make_constraint(F&& f, G&& g) noexcept {
        using ReturnType =
            detail::GenericConstraint<std::decay_t<F>, std::decay_t<G>>;
        struct {
          F f;
          G g;
        } capture{std::forward<F>(f), std::forward<G>(g)};
        return [already_called = false,
                capture = std::move(capture)]() mutable -> ReturnType {
          if (already_called)
            throw std::runtime_error(
                "Cannot call make_constraint factory more than once since "
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
      /*!\brief make_constraint() specialization for g moveable
       * \ingroup constraints
       *
       * \param f Value constraint functor
       * \param g Rate constraint functor
       * \return GenericConstraint initialized with f and g
       *
       * \see GenericConstraint
       */
      template <typename F, typename G,
                Requires<!make_constraint_detail::should_be_moved<F>> = nullptr,
                Requires<make_constraint_detail::should_be_moved<G>> = nullptr>
      auto make_constraint(F&& f, G&& g) noexcept {
        using ReturnType =
            detail::GenericConstraint<std::decay_t<F>, std::decay_t<G>>;
        struct {
          F f;
          G g;
        } capture{std::forward<F>(f), std::forward<G>(g)};
        return [already_called = false,
                capture = std::move(capture)]() mutable -> ReturnType {
          if (already_called)
            throw std::runtime_error(
                "Cannot call make_constraint factory more than once since "
                "the second functor is non-copyable and has been moved!");
          else {
            already_called = true;
            return ReturnType(std::move(capture.f), std::move(capture.g));
          }
        };
      }
      /*! \endcond */
      //************************************************************************

      /*
       * Although more elegant this version doesn't work because it requires
       * instantitaion of copy-constructors even for non-copyable types, unlike
       * if constexpr. So we have no option but to do SFINAE with
       * code-repetition
       */
      //    template <typename F, typename G>
      //    auto make_constraint(F&& f, G&& g) {
      //      using to_be_moved = std::true_type;
      //      using to_be_copied = std::false_type;
      //      using ReturnType =
      //          detail::GenericConstraint<std::decay_t<F>, std::decay_t<G>>;
      //
      //      struct {
      //        F f;
      //        G g;
      //      } capture{std::forward<F>(f), std::forward<G>(g)};
      //
      //      return make_overloader(
      //          // 1
      //          [capture = std::move(capture)](to_be_copied /* f */,
      //                                         to_be_copied /* g */) mutable {
      //            // is not noexcept given that the copy constructor can throw
      //            return [capture = std::move(capture)]() -> ReturnType
      //            {
      //              return ReturnType(capture.f, capture.g);
      //            };
      //          },
      //          // 2
      //          [capture = std::move(capture)](to_be_moved /* f */,
      //                                         to_be_copied /* g */) mutable {
      //            return [already_called = false,
      //                    capture = std::move(capture)]() mutable ->
      //                    ReturnType
      //                    {
      //              if (already_called)
      //                throw std::runtime_error(
      //                    "Cannot call maked constraint factory more than
      //                    once " "since the first functor is non-copyable and
      //                    has been " "moved!");
      //              else
      //                return ReturnType(std::move(capture.f),
      //                std::move(capture.g));
      //            };
      //          },
      //          // 3
      //          [capture = std::move(capture)](to_be_copied /* f */,
      //                                         to_be_moved /* g */) mutable {
      //            return [already_called = false,
      //                    capture = std::move(capture)]() mutable ->
      //                    ReturnType
      //                    {
      //              if (already_called)
      //                throw std::runtime_error(
      //                    "Cannot call maked constraint factory more than "
      //                    "once "
      //                    "since the second functor is non-copyable and has
      //                    been " "moved!");
      //              else
      //                return ReturnType(std::move(capture.f),
      //                std::move(capture.g));
      //            };
      //          },
      //          // 4
      //          [capture = std::move(capture)](to_be_moved /* f */,
      //                                         to_be_moved /* g */) mutable {
      //            return [already_called = false,
      //                    capture = std::move(capture)]() mutable ->
      //                    ReturnType
      //                    {
      //              if (already_called)
      //                throw std::runtime_error(
      //                    "Cannot call maked constraint factory more than "
      //                    "once "
      //                    "since both functors passed in are non-copyable and
      //                    have " "been " "moved already!");
      //              else
      //                return ReturnType(std::move(capture.f),
      //                std::move(capture.g));
      //            };
      //          })
      //          // make overloader arguments
      //          (make_constraint_detail::should_be_moved<F>{},
      //           make_constraint_detail::should_be_moved<G>{});
      //    }
    }  // namespace make_constraint_detail

    //**Type definitions********************************************************
    /*! \cond ELASTICA_INTERNAL */
    //! Strongly type value constraint
    template <typename Func>
    using ValueConstraint =
        named_type::NamedType<Func, struct ValueConstraintParameter,
                              named_type::Gettable>;

    //! Strongly typed rate constraint
    template <typename Func>
    using RateConstraint =
        named_type::NamedType<Func, struct RateConstraintParameter,
                              named_type::Gettable>;
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes a value constraint from a set of overloaded functors
     * \ingroup constraints
     *
     * \details
     * Marks the input functor f, others... as functions that can constrain
     * primitives within the context of an \elastica constraint.
     *
     * \example
     * The following shows a typical use of value constraint with a generic
     * lambda. Note that the lambda function needs to confirm to the expected
     * signature of a Constraint value function!
     * \snippet Test_MakeConstraint.cpp make_value_constraint_example
     *
     * However, you can also pass in several lambda functions, specialized for
     * different systems of your choosing:
     * \snippet Test_MakeConstraint.cpp make_value_constraint_overloaded_example
     *
     * \param f Input function that acts as a value constraint
     * \param funcs Input functions that act as a value constraint, for
     * overloading
     *
     * \see make_constraint()
     */
    template <typename F, typename... Funcs>
    constexpr inline decltype(auto)
    make_value_constraint(F&& f, Funcs&&... funcs) noexcept(
        noexcept(named_type::make_named_functor<ValueConstraint>(
            std::forward<F>(f), std::forward<Funcs>(funcs)...))) {
      return named_type::make_named_functor<ValueConstraint>(
          std::forward<F>(f), std::forward<Funcs>(funcs)...);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes a rate constraint from a set of overloaded functors
     * \ingroup constraints
     *
     * \details
     * Marks the input functor f, others... as functions that can constraint
     * rates within the context of an \elastica constraint.
     *
     * \example
     * The following shows a typical use of rate constraint with a generic
     * lambda. Note that the lambda function needs to confirm to the expected
     * signature of a Constraint rate function!
     * \snippet Test_MakeConstraint.cpp make_rate_constraint_example
     *
     * However, you can also pass in several lambda functions, specialized for
     * different systems of your choosing:
     * \snippet Test_MakeConstraint.cpp make_rate_constraint_overloaded_example
     *
     * \param f Input function that acts as a rate constraint
     * \param funcs Input functions that acts as a rate constraint
     *
     * \see make_constraint()
     */
    template <typename F, typename... Funcs>
    constexpr inline decltype(auto)
    make_rate_constraint(F&& f, Funcs&&... funcs) noexcept(
        noexcept(named_type::make_named_functor<RateConstraint>(
            std::forward<F>(f), std::forward<Funcs>(funcs)...))) {
      return named_type::make_named_functor<RateConstraint>(
          std::forward<F>(f), std::forward<Funcs>(funcs)...);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Makes an arbitrary \elastica constraint in an idiomatic way
     * \ingroup constraints
     *
     * \details
     * For rapidly prototyping constraints within \elastica, we have this
     * alternate mechanism instead of creating custom Constraint classes which
     * require a lot of boiler-plate code. The make_constraint() function takes
     * two  callables (differentiated appropriately into a value and rate
     * function, see below), with specific prototypes, and constructs a standard
     * \elastica Constraint with those functions. Usually, but not necessarily,
     * these callables are C++ lambda expressions. The following example is an
     * idiomatic way of creating a new Constraint
     *
     * \example
     * \snippet Test_MakeConstraint.cpp make_constraint_example
     *
     * We draw attention to the semantics and readability provided by using the
     * concomitant functions make_rate_constraint(), make_value_constraint().
     * Note that the order of make_rate_constraint() and make_value_constraint()
     * do not matter. Other use cases are possible, and we refer you to peruse
     * Test_MakeConstraint.cpp in the tests folders for patterns. Interestingly,
     * one can also  make parametrized constraints (i.e. constraints with
     * varying parameters), in the following fashion
     *
     * \snippet Test_MakeConstraint.cpp make_parametrized_constraint_example
     *
     * The above two should cover >90% of all use cases. Having introduced this
     * function, users are encouraged to write custom \elastica Constraint
     * classes only if
     *
     * - this does not satisfy their more complex use case
     * - runtime parameter support (loading from file, archiving it) is needed
     * - you want to record, document and reuse the custom Constraint class
     *
     * \param value_constraint A functor marked as value_constraint
     * \param rate_constraint A functor marked as rate_constraint
     * \return GenericConstraint initialized with `value_constraint` and
     * `rate_constraint`
     *
     * \see make_value_constraint(), make_rate_constraint()
     */
    template <typename F, typename G>
    auto make_constraint(
        ValueConstraint<F>&& value_constraint,
        RateConstraint<G>&&
            rate_constraint) noexcept(noexcept(make_constraint_detail::
                                                   make_constraint(
                                                       std::move(
                                                           value_constraint
                                                               .get()),
                                                       std::move(
                                                           rate_constraint
                                                               .get())))) {
      return make_constraint_detail::make_constraint(
          std::move(value_constraint.get()), std::move(rate_constraint.get()));
    }
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of make_constraint for switched arguments
     * \ingroup constraints
     */
    template <typename F, typename G>
    auto make_constraint(
        RateConstraint<G>&& rate_constraint,
        ValueConstraint<F>&&
            value_constraint) noexcept(noexcept(make_constraint_detail::
                                                    make_constraint(
                                                        std::move(
                                                            value_constraint
                                                                .get()),
                                                        std::move(
                                                            rate_constraint
                                                                .get())))) {
      return make_constraint_detail::make_constraint(
          std::move(value_constraint.get()), std::move(rate_constraint.get()));
    }
    /*! \endcond */
    //**************************************************************************

  }  // namespace constraints

}  // namespace elastica