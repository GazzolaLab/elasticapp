//==============================================================================
/*!
//  \file
//  \brief Algorithm execution policies for the \elastica Systems module
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
#include <algorithm>
#include <tuple>  // for construction support
#include <utility>

///
#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"
#include "Simulator/Interface/Systems/Types.hpp"
///
#include "Parallel/for_each.hpp"  // uses elastica's, not GLIBCXX
#include "Parallel/invoke.hpp"
///
#include "Utilities/CRTP.hpp"
#include "Utilities/ForceInline.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/MakeFromTuple.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace interface {

    namespace systems {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      namespace detail {
#define FWD(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)
        template <typename... Ts>
        auto fwd_capture(Ts&&... xs) {
          return std::tuple<Ts...>(FWD(xs)...);
        }
        template <typename T>
        decltype(auto) access(T&& x) {
          return std::get<0>(FWD(x));
        }
      }  // namespace detail
#define FWD_CAPTURE(...) detail::fwd_capture(FWD(__VA_ARGS__))

      //************************************************************************
      /*!\brief Execution policy for Systems module
       * \ingroup interface
       *
       * This class represents the policy of execution for applying systems
       * to systems within the ambit of the systems module. As such, the
       * policies span a spectrum for purely sequential execution to parallel,
       * asynchronous execution.
       *
       * \tparam SystemsTraits Options for configuration of the Systems
       * Module
       * \tparam SystemsModule The systems module of the Simulator
       *
       * \see elastica::interface::Systems
       */
      template <class SystemsTraits, class SystemsModule>
      class ExecutionPolicy
          : public CRTPHelper<SystemsModule, ExecutionPolicy> {
       private:
        //**Type definitions****************************************************
        //! CRTP Type
        using CRTP = CRTPHelper<SystemsModule, ExecutionPolicy>;
        //! CRTP methods
        using CRTP::self;
        //! Type of simulation time
        using TimeType = typename SystemsTraits::TimeType;
        //**********************************************************************

       protected:
        //**Type definitions****************************************************
        //! Operator for invoking a functor over one system collection
        template <typename Policy, class Dummy = void>
        struct ForEachSystemOp;
        //! Base Operator for invoking a functor across system types
        template <typename OverSysPolicy, class Dummy = void>
        struct InvokeOverSystemsOpBase;
        //! Operator for invoking a functor across system types
        template <typename ForEachSysPolicy, typename OverSysPolicy>
        struct InvokeOverSystemsOp;
        //**********************************************************************

       protected:
        //**For each functors***************************************************
        /*!\name For each functors */
        //@{

        //**********************************************************************
        /*!\brief Apply over systems within a collection, in sequence
         */
        template <class Dummy>
        struct ForEachSystemOp<::elastica::configuration::sequential_policy,
                               Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ForEachSystemOp(SystemsModule* /*meta*/) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ForEachSystemOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute func on systems
           *
           * \param func Function to be applied over systems
           */
          template <typename F>
          ELASTICA_ALWAYS_INLINE constexpr auto operator()(
              F&& func) const noexcept {
            /*
             * \param system_collection (Iterable) collection of systems of one
             * type
             */
            /*
             * \note
             * Perfect forwarding doesn't work here because it needs to be
             * mutable, but then since the member is const it throws a weird
             * error.
             */
            return [func = FWD_CAPTURE(func)](
                       auto /*const*/& system_collection) mutable
                   -> void /*decltype(auto)*/ {
              // parallelism should go here
              using std::begin;
              using std::end;
              std::for_each(begin(system_collection), end(system_collection),
                            // If rvalue, func is a tuple of the type so we can
                            // move If lvalue, func is a tuple of the lvalue of
                            // the type so move does nothing really
                            std::move(detail::access(func)));
              // std::forward<F>(func));
            };
          }
          //********************************************************************
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply over systems within a collection, in parallel
         */
        template <class Dummy>
        struct ForEachSystemOp<::elastica::configuration::parallel_policy,
                               Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ForEachSystemOp(SystemsModule* /*meta*/) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ForEachSystemOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute func on systems
           *
           * \param func Function to be applied over systems
           */
          template <typename F>
          ELASTICA_ALWAYS_INLINE constexpr auto operator()(
              F&& func) const noexcept {
            /*
             * \param system_collection (Iterable) collection of systems of one
             * type
             */
            /*
             * \note
             * Perfect forwarding doesn't work here because it needs to be
             * mutable, but then since the member is const it throws a weird
             * error.
             * [func = std::forward<F>(func)]
             */
            return [func = FWD_CAPTURE(func)](
                       auto /*const*/& system_collection) mutable
                   -> void /*decltype(auto)*/ {
              // parallelism should go here
              using ::elastica::size;
              // since its not mutable, calls the copy-constructor always
              // func is copied here, so its state may not be preserved
              ::elastica::parallel::for_each(
                  0UL, size(system_collection),
                  // same logic as sequential for moving it
                  [&system_collection, f = detail::access(func)](
                      std::size_t const idx) { f(system_collection[idx]); });
              // detail::access returns a & to a non-existing value, which is
              // dangerous
              // return static_cast<std::decay_t<F>>(detail::access(func));
              // std::forward<F>(func));
            };
          }
          //********************************************************************
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply over systems within a collection, as a hybrid between
         * sequential and parallel execution
         */
        template <class Dummy>
        struct ForEachSystemOp<::elastica::configuration::hybrid_policy, Dummy>
            : public ForEachSystemOp<
                  ::elastica::configuration::sequential_policy>,
              public ForEachSystemOp<
                  ::elastica::configuration::parallel_policy> {
          //**Type definitions**************************************************
          //! Sequential connection op
          using ForEachSystemInSequence =
              ForEachSystemOp<::elastica::configuration::sequential_policy>;
          //! Parallel connection op
          using ForEachSystemInParallel =
              ForEachSystemOp<::elastica::configuration::parallel_policy>;
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor.
           */
          ForEachSystemOp(SystemsModule* sim)
              : ForEachSystemInSequence(sim),
                ForEachSystemInParallel(sim),
                module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ForEachSystemOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute func on systems
           *
           * \param func Function to be applied over systems
           */
          template <typename F>
          ELASTICA_ALWAYS_INLINE constexpr auto operator()(
              F&& func) const noexcept {
            /*
             * \param system_collection (Iterable) collection of systems of one
             * type
             */
            return [&, func = FWD_CAPTURE(func)](
                       auto /*const*/& system_collection) /*mutable*/
                   -> void /*decltype(auto)*/ {
              // Is this if() check viable when put in a flow graph?
              if (module_handle.to_be_iterated_sequentially(system_collection))
                ForEachSystemInSequence::operator()(detail::access(func))(
                    system_collection);
              else
                ForEachSystemInParallel::operator()(detail::access(func))(
                    system_collection);
            };
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the ConnectionsModule
          SystemsModule const& module_handle;
          //********************************************************************
        };
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Invoke functor******************************************************
        /*!\name Invoke functors */
        //@{

        //**********************************************************************
        /*!\brief Invoke over system types in sequence
         */
        template <class Dummy>
        struct InvokeOverSystemsOpBase<
            ::elastica::configuration::sequential_policy, Dummy> {
          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          InvokeOverSystemsOpBase() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~InvokeOverSystemsOpBase() = default;
          //********************************************************************

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Operation to invoke on multiple types of system
           *
           * \param for_each_op Callable to be applied over system types
           */
          template <typename ForEachOp>
          ELASTICA_ALWAYS_INLINE constexpr auto invoke_over_systems(
              ForEachOp&& for_each_op) const noexcept {
            return
                [op_for_each = FWD_CAPTURE(for_each_op)](
                    auto&... system_collections) mutable -> void /*ForEach*/ {
                  /* 1. Needs func to be assignable, NRVO should kick in to move
                   * from for_each_system
                   * 2. evaluation func_for_each(system) gives back func and not
                   * func_for_each, so we cannot assign it to func for each.
                   */
                  // EXPAND_PACK_LEFT_TO_RIGHT(
                  //     (func_for_each = func_for_each(system_collections)));

                  // TODO : Whats the performance impact of std::ref, since this
                  // is going to be a tight loop?
                  EXPAND_PACK_LEFT_TO_RIGHT(
                      detail::access(op_for_each)(system_collections));

                  // cpp17 fold style
                  // ((void)(func(decltype(system_collections)(system_collections))),
                  // ...);

                  // TODO : why doesnt this work? shouldnt NRVO kick in?
                  // return func_for_each;
                  // return static_cast<F&&>(func);
                };
          }
          /*! \endcond */
          //********************************************************************
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Invoke over system types in parallel
         */
        template <class Dummy>
        struct InvokeOverSystemsOpBase<
            ::elastica::configuration::parallel_policy, Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          InvokeOverSystemsOpBase() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~InvokeOverSystemsOpBase() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute a callable over all types of systems
           *
           * \param for_each_op Callable to be applied over each system
           */
          template <typename ForEachOp>
          ELASTICA_ALWAYS_INLINE constexpr auto invoke_over_systems(
              ForEachOp&& for_each_op) const noexcept {
            return
                [op_for_each = FWD_CAPTURE(for_each_op)](
                    auto&... system_collections) mutable -> void /*ForEachOp*/ {
                  // op_for_each is copied to each thread here, so pay attention
                  // to state
                  parallel::invoke([op_for_each = detail::access(op_for_each),
                                    &system_collections]() mutable {
                    op_for_each(system_collections);
                  }...);
                  //  EXPAND_PACK_LEFT_TO_RIGHT(
                  //      detail::access(op_for_each)(system_collections));
                };
          }
          //********************************************************************
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Invoke over system types
         *
         * Provides abstraction from the actual storage (tuple/tagged tuple)
         * while only the implementation details (parallelism) are found
         * upstream
         */
        template <typename ForEachSysPolicy, typename AcrossSysPolicy>
        struct InvokeOverSystemsOp
            : public InvokeOverSystemsOpBase<AcrossSysPolicy> {
          //**Type definitions**************************************************
          using Op = InvokeOverSystemsOpBase<AcrossSysPolicy>;
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor.
           */
          explicit InvokeOverSystemsOp(SystemsModule* sim, SystemsTraits const&)
              : Op(), module_handle(*sim), for_each_system(sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit InvokeOverSystemsOp(
              std::tuple<SystemsModule*, SystemsTraits const&>&& args)
              : InvokeOverSystemsOp(cpp17::make_from_tuple<InvokeOverSystemsOp>(
                    std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~InvokeOverSystemsOp() = default;
          //********************************************************************

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Execute a callable over all types of systems
           *
           * \param func Callable to be applied over system types
           */
          using Op::invoke_over_systems;
          /*! \endcond */
          //********************************************************************

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Execute a callable over a single type of system
           *
           * \param func Callable to be applied over one system type
           */
          template <typename F, typename SystemsBlock>
          inline auto subset_for_each(F do_func,
                                      tmpl::list<SystemsBlock>) const noexcept
              -> F {
            // this cannot happen serially, so push resposibility to for_each
            for_each_system(std::ref(do_func))(
                module_handle
                    .template get_system_collection_container<SystemsBlock>());
            return do_func;
          }
          /*! \endcond */
          //********************************************************************

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Execute a callable over multiple, non-exhaustive types of
           * systems
           *
           * \details
           * Called when >1 type is requested, but not all types.
           *
           * \param func Callable to be applied over system types
           */
          template <typename F, typename... SystemsBlocks>
          inline auto subset_for_each(
              F func, tmpl::list<SystemsBlocks...>) const noexcept -> F {
            // TODO : Whats the performance impact of std::ref, since this
            // is going to be a tight loop?
            (void)cpp17::apply(
                invoke_over_systems(for_each_system(std::ref(func))),
                std::tie(module_handle.template get_system_collection_container<
                         SystemsBlocks>()...));
            return func;
          }
          /*! \endcond */
          //********************************************************************

          //********************************************************************
          /*!\brief Execute a callable over a subset of system types
           *
           * \tparam BlocksTypeList The subset types of systems over which
           * to apply the Callable `func`
           * \param func Callable to be applied over a subset of system types
           */
          template <typename BlocksTypeList, typename F>
          inline decltype(auto) operator()(F&& func) const noexcept {
            return subset_for_each(std::forward<F>(func), BlocksTypeList{});
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Execute a callable over all types of systems
           *
           * \param func Callable to be applied over system types
           */
          template <typename F>
          inline auto operator()(F func) const noexcept -> F {
            // TODO : Whats the performance impact of std::ref, since this
            // is going to be a tight loop?
            (void)cpp17::apply(
                invoke_over_systems(for_each_system(std::ref(func))),
                module_handle.get_system_collection_container());
            return func;
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the SystemsModule
          SystemsModule /*const*/& module_handle;
          //! Functor to be applied over individual systems
          ForEachSystemOp<ForEachSysPolicy> const for_each_system;
          //********************************************************************
        };
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**Algorithms**********************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\name Algorithms for connections*/
        //@{
        struct ApplyOverSystems {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type =
              InvokeOverSystemsOp<typename SystemsTraits::IterationPolicy::
                                      ExecutionPolicyForEachSystemType,
                                  typename SystemsTraits::IterationPolicy::
                                      ExecutionPolicyAcrossSystemTypes>;
          //********************************************************************
        };
        //@}
        /*! \endcond */
        //**********************************************************************

       public:
        //**Algorithmic order***************************************************
        /*!\name Algorithmic steps within a temporal loop*/
        //@{
        //! Algorithms to be carried out as an update step
        using UpdateSteps = tmpl::list<ApplyOverSystems>;
        //@}
        //**********************************************************************
      };
      //************************************************************************

#undef FWD_CAPTURE
#undef FWD

    }  // namespace systems

  }  // namespace interface

}  // namespace elastica