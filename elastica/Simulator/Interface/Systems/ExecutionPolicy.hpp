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
#include "Simulator/Configuration/ModuleConfiguration/Streams.hpp"  // string
#include "Simulator/Interface/Affordances/detail/EmptyUpdateSteps.hpp"
#include "Simulator/Interface/Systems/Types.hpp"
///
#include "Parallel/for_each.hpp"  // uses elastica's, not GLIBCXX
#include "Parallel/invoke.hpp"
///
#include "Utilities/CRTP.hpp"
#include "Utilities/ForceInline.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/MakeFromTuple.hpp"
#include "Utilities/MakeString.hpp"
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
      inline std::string update_kinematics_algorithm_desc(
          std::size_t n_systems) {
        return std::string(MakeString{}
                           << "Update kinematic (position, director) states of "
                           << n_systems << " systems");
      }

      inline std::string update_dynamics_algorithm_desc(std::size_t n_systems) {
        return std::string(
            MakeString{}
            << "Update dynamic (velocity, angular velocity) states of "
            << n_systems << " systems");
      }

      inline std::string compute_internal_dynamics_algorithm_desc(
          std::size_t n_systems) noexcept {
        return std::string(MakeString{}
                           << "Compute internal dynamics (forces, torques) of "
                           << n_systems << " systems");
      }

      inline std::string update_algorithm_desc(std::size_t n_systems) noexcept {
        return std::string(MakeString{} << "Update " << n_systems
                                        << " systems");
      }
      //************************************************************************

      //**********************************************************************
#define APPEND_POLICY_TO_DESC(func)                        \
  template <typename ForEachPolicy, typename AcrossPolicy> \
  inline std::string func(std::size_t n_constraints) {     \
    return func(n_constraints) + " using " +               \
           configuration::to_string(ForEachPolicy{});      \
  }
      APPEND_POLICY_TO_DESC(update_kinematics_algorithm_desc)
      APPEND_POLICY_TO_DESC(update_dynamics_algorithm_desc)
      APPEND_POLICY_TO_DESC(compute_internal_dynamics_algorithm_desc)
      APPEND_POLICY_TO_DESC(update_algorithm_desc)
#undef APPEND_POLICY_TO_DESC
      //**********************************************************************

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
          : public CRTPHelper<SystemsModule, ExecutionPolicy>,
            public ::elastica::modules::detail::EmptyUpdateSteps<
                SystemsTraits, ExecutionPolicy<SystemsTraits, SystemsModule>> {
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
              F func) const noexcept {
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
            return [func = std::move(func)](
                       auto /*const*/& system_collection) /*mutable*/
                   -> void /*decltype(auto)*/ {
              // parallelism should go here
              using std::begin;
              using std::end;
              std::for_each(begin(system_collection), end(system_collection),
                            // If rvalue, func is a tuple of the type so we can
                            // move If lvalue, func is a tuple of the lvalue of
                            // the type so move does nothing really
                            std::ref(func));
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
              F func) const noexcept {
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
            return [func = std::move(func)](
                       auto /*const*/& system_collection) mutable
                   -> void /*decltype(auto)*/ {
              // parallelism should go here
              using ::elastica::size;
              // since its not mutable, calls the copy-constructor always
              // func is copied here, so its state may not be preserved
              ::elastica::parallel::for_each(
                  system_collection,
                  // same logic as sequential for moving it
                  // Moving is fine here for two reasons :
                  // 1. Within for_each, f gets copy constructed within the
                  // threads from the lambda passed to for_each below
                  // 2. In invoke_over, each parallel invoker gets its own copy
                  // of lambda (whose scope we are in)---hence its safe to move
                  // into the next lambda.
                  [f = std::move(func)](auto& curr_system) { f(curr_system); });
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
              F func) const noexcept {
            /*
             * \param system_collection (Iterable) collection of systems of one
             * type
             */
            return [&, func = std::move(func)](
                       auto /*const*/& system_collection) mutable
                   -> void /*decltype(auto)*/ {
              // Is this if() check viable when put in a flow graph?
              if (module_handle.to_be_iterated_sequentially(system_collection))
                ForEachSystemInSequence::operator()(std::move(func))(
                    system_collection);
              else
                ForEachSystemInParallel::operator()(std::move(func))(
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
              ForEachOp for_each_op) const noexcept {
            return [op_for_each = std::move(for_each_op)](
                       auto&... system_collections) -> void /*ForEach*/ {
              /* 1. Needs func to be assignable, NRVO should kick in to move
               * from for_each_system
               * 2. evaluation func_for_each(system) gives back func and not
               * func_for_each, so we cannot assign it to func for each.
               */
              // EXPAND_PACK_LEFT_TO_RIGHT(
              //     (func_for_each = func_for_each(system_collections)));

              // Make one copy for each system invocation before launching it
              // op_for_each can modify its state (by moving resources) after
              // every call, so create copies here.

              //              EXPAND_PACK_LEFT_TO_RIGHT(
              //                  [op = op_for_each, &system_collections]() {
              //                    op(system_collections);
              //                  }());

              // For the same reason as parallel invoke, we explicitly ensure
              // that this lambda is NOT maked mutable by having a buffer of
              // ops and systems.
              // This allows execution of lambdas marked const (as it should)
              // without compilation issues.

              constexpr auto n_system_types = sizeof...(system_collections);
              /*constexpr*/ auto operations(
                  make_array<n_system_types>(std::move(op_for_each)));
              /*constexpr*/ auto systems(std::tie(system_collections...));

              auto form_function = [&](auto Is) noexcept {
                return [&op = std::get<Is>(operations),
                        &system = std::get<Is>(systems)]() { op(system); };
              };
              // Form function and immediately execute.
              index_apply<n_system_types>([&](auto... Is) {
                EXPAND_PACK_LEFT_TO_RIGHT(form_function(Is)());
              });

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
              ForEachOp for_each_op) const noexcept {
            return [op_for_each = std::move(for_each_op)](
                       auto&... system_collections) -> void /*ForEachOp*/ {
              // op_for_each is copied to each thread here, so pay attention
              // to state
              // TBB parallel invoke does not copy its functors by design.
              // Method is marked mutable for the same reason as sequential:
              // each captured op's operator() is mutable (for moving
              // resources). op is captured by value, and so the lambda below is
              // indirectly changed by op.

              /// See explanation below.
              // parallel::invoke(
              //     [op = op_for_each, &system_collections]() mutable {
              //       op(system_collections);
              //     }...);

              // TBB parallel invoke necessitates that function is marked
              // const(), but our op is allowed to change state. To WAR this
              // issue, we create n_copies of op and refer to them instead.
              // using Ops = std::array<decltype(op), >;
              constexpr auto n_system_types = sizeof...(system_collections);
              // op_for_each may not have constexpr constructors
              /*constexpr*/ auto operations(
                  make_array<n_system_types>(std::move(op_for_each)));
              // Form a tuple of references, system_collections is not constexpr
              // as it is a function parameter.
              /*constexpr*/ auto systems(std::tie(system_collections...));

              auto form_function = [&](auto Is) noexcept {
                return [&op = std::get<Is>(operations),
                        &system = std::get<Is>(systems)]() { op(system); };
              };

              index_apply<n_system_types>(
                  [&](auto... Is) { parallel::invoke(form_function(Is)...); });

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
          inline void subset_for_each(F do_func,
                                      tmpl::list<SystemsBlock>) const noexcept {
            // this cannot happen serially, so push resposibility to for_each
            for_each_system(std::move(do_func))(
                module_handle
                    .template get_system_collection_container<SystemsBlock>());
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
          inline void subset_for_each(
              F func, tmpl::list<SystemsBlocks...>) const noexcept {
            (void)cpp17::apply(
                invoke_over_systems(for_each_system(std::move(func))),
                std::tie(module_handle.template get_system_collection_container<
                         SystemsBlocks>()...));
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
          inline void operator()(F func) const noexcept {
            subset_for_each(std::move(func), BlocksTypeList{});
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Execute a callable over all types of systems
           *
           * \param func Callable to be applied over system types
           */
          template <typename F>
          inline void operator()(F func) const noexcept {
            cpp17::apply(invoke_over_systems(for_each_system(std::move(func))),
                         module_handle.get_system_collection_container());
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Stream operator
           */
          friend std::ostream& operator<<(std::ostream& os,
                                          InvokeOverSystemsOp const& op) {
            return os
                   << update_algorithm_desc<ForEachSysPolicy, AcrossSysPolicy>(
                          op.module_handle.n_systems());
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

        //**Invoke tags*********************************************************
        /*!\name Tag for invoke functors */
        //@{
        struct UpdateKinematicsTag {};
        struct ComputeInternalDynamicsTag {};
        struct UpdateDynamicsTag {};
        //@}
        //**********************************************************************

        template <typename Tag>
        struct TaggedInvoke {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          explicit TaggedInvoke(SystemsModule* sim, SystemsTraits const&)
              : module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit TaggedInvoke(
              std::tuple<SystemsModule*, SystemsTraits const&> args)
              : TaggedInvoke(cpp17::make_from_tuple<TaggedInvoke<Tag>>(
                    std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Execute a callable over all types of systems
           *
           * \param func Callable to be applied over system types
           */
          template <typename F>
          inline void operator()(F func) const noexcept {
            return module_handle.for_each_block_untimed(std::move(func));
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~TaggedInvoke() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Stream operator
           */
          friend std::ostream& operator<<(std::ostream& os,
                                          TaggedInvoke const& op) {
            const auto n_systems = op.module_handle.n_systems();
            return make_overloader(
                [=, &os](UpdateKinematicsTag /*meta*/) -> std::ostream& {
                  return os << update_kinematics_algorithm_desc(n_systems);
                },
                [=, &os](ComputeInternalDynamicsTag /*meta*/) -> std::ostream& {
                  return os
                         << compute_internal_dynamics_algorithm_desc(n_systems);
                },
                [=, &os](UpdateDynamicsTag /*meta*/) -> std::ostream& {
                  return os << update_dynamics_algorithm_desc(n_systems);
                })(Tag{});
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the SystemsModule
          SystemsModule /*const*/& module_handle;
          //********************************************************************
        };

        //@}
        //**********************************************************************

       protected:
        //**Algorithms**********************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\name Algorithms for systems*/
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

          //**Info method*******************************************************
          static std::string metadata(
              SystemsModule const& module_handle) noexcept {
            return update_algorithm_desc<
                typename SystemsTraits::IterationPolicy::
                    ExecutionPolicyForEachSystemType,
                typename SystemsTraits::IterationPolicy::
                    ExecutionPolicyAcrossSystemTypes>(
                module_handle.n_systems());
          }
          //********************************************************************
        };

        struct ApplyOverSystemsSeq {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type =
              InvokeOverSystemsOp<::elastica::configuration::sequential_policy,
                                  ::elastica::configuration::sequential_policy>;
          //********************************************************************

          //**Info method*******************************************************
          static std::string metadata(
              SystemsModule const& module_handle) noexcept {
            return "Sequential fallback algorithm to " +
                   update_algorithm_desc<
                       ::elastica::configuration::sequential_policy,
                       ::elastica::configuration::sequential_policy>(
                       module_handle.n_systems());
          }
          //********************************************************************
        };

        struct UpdateKinematics {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = TaggedInvoke<UpdateKinematicsTag>;
          //********************************************************************

          //**Info method*******************************************************
          static std::string metadata(
              SystemsModule const& module_handle) noexcept {
            return update_kinematics_algorithm_desc<
                typename SystemsTraits::IterationPolicy::
                    ExecutionPolicyForEachSystemType,
                typename SystemsTraits::IterationPolicy::
                    ExecutionPolicyAcrossSystemTypes>(
                module_handle.n_systems());
          }
          //********************************************************************
        };

        struct ComputeInternalDynamics {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = TaggedInvoke<ComputeInternalDynamicsTag>;
          //********************************************************************

          //**Info method*******************************************************
          static std::string metadata(
              SystemsModule const& module_handle) noexcept {
            return compute_internal_dynamics_algorithm_desc<
                typename SystemsTraits::IterationPolicy::
                    ExecutionPolicyForEachSystemType,
                typename SystemsTraits::IterationPolicy::
                    ExecutionPolicyAcrossSystemTypes>(
                module_handle.n_systems());
          }
          //********************************************************************
        };

        struct UpdateDynamics {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = TaggedInvoke<UpdateDynamicsTag>;
          //********************************************************************

          //**Info method*******************************************************
          static std::string metadata(
              SystemsModule const& module_handle) noexcept {
            return update_dynamics_algorithm_desc<
                typename SystemsTraits::IterationPolicy::
                    ExecutionPolicyForEachSystemType,
                typename SystemsTraits::IterationPolicy::
                    ExecutionPolicyAcrossSystemTypes>(
                module_handle.n_systems());
          }
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
        using UpdateSteps =
            tmpl::list<ApplyOverSystems, ApplyOverSystemsSeq, UpdateKinematics,
                       ComputeInternalDynamics, UpdateDynamics>;
        //@}
        //**********************************************************************
      };
      //************************************************************************

#undef FWD_CAPTURE
#undef FWD

    }  // namespace systems

  }  // namespace interface

}  // namespace elastica