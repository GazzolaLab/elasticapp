//==============================================================================
/*!
//  \file
//  \brief Algorithm execution policies for the \elastica Primitives module
//  \note The contents of this file are mostly a copy of Systems/ExecutionPolicy
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
#include "Simulator/Interface/Environments/Types.hpp"
///
#include "Parallel/for_each.hpp"  // uses elastica's, not GLIBCXX
#include "Parallel/invoke.hpp"
///
#include "Utilities/CRTP.hpp"
#include "Utilities/ForceInline.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/MakeFromTuple.hpp"
#include "Utilities/MakeString.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

namespace elastica {

  namespace interface {

    namespace environments {

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
      /*!\brief Execution policy for Primitives module
       * \ingroup interface
       *
       * This class represents the policy of execution for applying primitives
       * to primitives within the ambit of the primitives module. As such, the
       * policies span a spectrum for purely sequential execution to parallel,
       * asynchronous execution.
       *
       * \tparam PrimitivesTraits Options for configuration of the Primitives
       * Module
       * \tparam PrimitivesModule The primitives module of the Simulator
       *
       * \see elastica::interface::Primitives
       */
      template <class PrimitivesTraits, class PrimitivesModule>
      class WithPrimitivesExecutionPolicy
          : public CRTPHelper<PrimitivesModule, WithPrimitivesExecutionPolicy> {
       private:
        //**Type definitions****************************************************
        //! CRTP Type
        using CRTP =
            CRTPHelper<PrimitivesModule, WithPrimitivesExecutionPolicy>;
        //! CRTP methods
        using CRTP::self;
        //! Type of simulation time
        using TimeType = typename PrimitivesTraits::TimeType;
        //**********************************************************************

       protected:
        //**Type definitions****************************************************
        //! Operator for invoking a functor over one primitive collection
        template <typename Policy, class Dummy = void>
        struct ForEachPrimitiveOp;
        //! Base Operator for invoking a functor across primitive types
        template <typename OverPrimPolicy, class Dummy = void>
        struct InvokeOverPrimitivesOpBase;
        //! Operator for invoking a functor across primitive types
        template <typename ForEachPrimPolicy, typename OverPrimPolicy>
        struct InvokeOverPrimitivesOp;
        //**********************************************************************

       protected:
        //**********************************************************************
        static std::string update_primitive_algorithm_desc(
            std::size_t n_primitives) noexcept {
          return (MakeString{} << "Update " << n_primitives
                               << " primitives by ");
        }
        //**********************************************************************

        //**********************************************************************
        template <typename ForEachPolicy, typename AcrossPolicy>
        static std::string update_primitive_algorithm_desc(
            std::size_t n_primitives) {
          return update_primitive_algorithm_desc(n_primitives) + " using " +
                 configuration::to_string(ForEachPolicy{});
        }
        //**********************************************************************

       protected:
        //**For each functors***************************************************
        /*!\name For each functors */
        //@{

        //**********************************************************************
        /*!\brief Apply over primitives within a collection, in sequence
         */
        template <class Dummy>
        struct ForEachPrimitiveOp<::elastica::configuration::sequential_policy,
                                  Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ForEachPrimitiveOp(PrimitivesModule* /*meta*/) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ForEachPrimitiveOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute func on primitives
           *
           * \param func Function to be applied over primitives
           */
          template <typename F>
          ELASTICA_ALWAYS_INLINE constexpr auto operator()(
              F&& func) const noexcept {
            /*
             * \param primitive_collection (Iterable) collection of primitives
             * of one type
             */
            /*
             * \note
             * Perfect forwarding doesn't work here because it needs to be
             * mutable, but then since the member is const it throws a weird
             * error.
             */
            return [func = FWD_CAPTURE(func)](
                       auto /*const*/& primitive_collection) /*mutable*/
                   -> void /*decltype(auto)*/ {
              // parallelism should go here
              using std::begin;
              using std::end;
              std::for_each(begin(primitive_collection),
                            end(primitive_collection),
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
        /*!\brief Apply over primitives within a collection, in parallel
         */
        template <class Dummy>
        struct ForEachPrimitiveOp<::elastica::configuration::parallel_policy,
                                  Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          ForEachPrimitiveOp(PrimitivesModule* /*meta*/) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ForEachPrimitiveOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute func on primitives
           *
           * \param func Function to be applied over primitives
           */
          template <typename F>
          ELASTICA_ALWAYS_INLINE constexpr auto operator()(
              F&& func) const noexcept {
            /*
             * \param primitive_collection (Iterable) collection of primitives
             * of one type
             */
            /*
             * \note
             * Perfect forwarding doesn't work here because it needs to be
             * mutable, but then since the member is const it throws a weird
             * error.
             * [func = std::forward<F>(func)]
             */
            return [func = FWD_CAPTURE(func)](
                       auto /*const*/& primitive_collection) /*mutable*/
                   -> void /*decltype(auto)*/ {
              // parallelism should go here
              using ::elastica::size;
              // since its not mutable, calls the copy-constructor always
              // func is copied here, so its state may not be preserved
              ::elastica::parallel::for_each(
                  primitive_collection,
                  // same logic as sequential for moving it
                  [f = detail::access(func)](auto& curr_primitive) {
                    f(curr_primitive);
                  });
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
        /*!\brief Apply over primitives within a collection, as a hybrid between
         * sequential and parallel execution
         */
        template <class Dummy>
        struct ForEachPrimitiveOp<::elastica::configuration::hybrid_policy,
                                  Dummy>
            : public ForEachPrimitiveOp<
                  ::elastica::configuration::sequential_policy>,
              public ForEachPrimitiveOp<
                  ::elastica::configuration::parallel_policy> {
          //**Type definitions**************************************************
          //! Sequential connection op
          using ForEachPrimitiveInSequence =
              ForEachPrimitiveOp<::elastica::configuration::sequential_policy>;
          //! Parallel connection op
          using ForEachPrimitiveInParallel =
              ForEachPrimitiveOp<::elastica::configuration::parallel_policy>;
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor.
           */
          ForEachPrimitiveOp(PrimitivesModule* sim)
              : ForEachPrimitiveInSequence(sim),
                ForEachPrimitiveInParallel(sim),
                module_handle(*sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~ForEachPrimitiveOp() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute func on primitives
           *
           * \param func Function to be applied over primitives
           */
          template <typename F>
          ELASTICA_ALWAYS_INLINE constexpr auto operator()(
              F&& func) const noexcept {
            /*
             * \param primitive_collection (Iterable) collection of primitives
             * of one type
             */
            return [&, func = FWD_CAPTURE(func)](
                       auto /*const*/& primitive_collection) /*mutable*/
                   -> void /*decltype(auto)*/ {
              // Is this if() check viable when put in a flow graph?
              if (module_handle.to_be_iterated_sequentially(
                      primitive_collection))
                ForEachPrimitiveInSequence::operator()(detail::access(func))(
                    primitive_collection);
              else
                ForEachPrimitiveInParallel::operator()(detail::access(func))(
                    primitive_collection);
            };
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the ConnectionsModule
          PrimitivesModule const& module_handle;
          //********************************************************************
        };
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Invoke functor******************************************************
        /*!\name Invoke functors */
        //@{

        //**********************************************************************
        /*!\brief Invoke over primitive types in sequence
         */
        template <class Dummy>
        struct InvokeOverPrimitivesOpBase<
            ::elastica::configuration::sequential_policy, Dummy> {
          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          InvokeOverPrimitivesOpBase() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~InvokeOverPrimitivesOpBase() = default;
          //********************************************************************

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Operation to invoke on multiple types of primitive
           *
           * \param for_each_op Callable to be applied over primitive types
           */
          template <typename ForEachOp>
          ELASTICA_ALWAYS_INLINE constexpr auto invoke_over_primitives(
              ForEachOp&& for_each_op) const noexcept {
            return [op_for_each = FWD_CAPTURE(for_each_op)](
                       auto&... primitive_collections) mutable
                   -> void /*ForEach*/ {
              /* 1. Needs func to be assignable, NRVO should kick in to move
               * from for_each_primitive
               * 2. evaluation func_for_each(primitive) gives back func and not
               * func_for_each, so we cannot assign it to func for each.
               */
              // EXPAND_PACK_LEFT_TO_RIGHT(
              //     (func_for_each = func_for_each(primitive_collections)));

              // TODO : Whats the performance impact of std::ref, since this
              // is going to be a tight loop?
              EXPAND_PACK_LEFT_TO_RIGHT(
                  detail::access(op_for_each)(primitive_collections));

              // cpp17 fold style
              // ((void)(func(decltype(primitive_collections)(primitive_collections))),
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
        /*!\brief Invoke over primitive types in parallel
         */
        template <class Dummy>
        struct InvokeOverPrimitivesOpBase<
            ::elastica::configuration::parallel_policy, Dummy> {
          //********************************************************************
          /*!\brief Policy constructor.
           */
          InvokeOverPrimitivesOpBase() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~InvokeOverPrimitivesOpBase() = default;
          //********************************************************************

          //********************************************************************
          /*!\brief Execute a callable over all types of primitives
           *
           * \param for_each_op Callable to be applied over each primitive
           */
          template <typename ForEachOp>
          ELASTICA_ALWAYS_INLINE constexpr auto invoke_over_primitives(
              ForEachOp&& for_each_op) const noexcept {
            return [op_for_each = FWD_CAPTURE(for_each_op)](
                       auto&... primitive_collections) mutable
                   -> void /*ForEachOp*/ {
              // op_for_each is copied to each thread here, so pay attention
              // to state
              parallel::invoke([op_for_each = detail::access(op_for_each),
                                &primitive_collections]() /*mutable*/ {
                op_for_each(primitive_collections);
              }...);
              //  EXPAND_PACK_LEFT_TO_RIGHT(
              //      detail::access(op_for_each)(primitive_collections));
            };
          }
          //********************************************************************
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Invoke over primitive types
         *
         * Provides abstraction from the actual storage (tuple/tagged tuple)
         * while only the implementation details (parallelism) are found
         * upstream
         */
        template <typename ForEachPrimPolicy, typename AcrossPrimPolicy>
        struct InvokeOverPrimitivesOp
            : public InvokeOverPrimitivesOpBase<AcrossPrimPolicy> {
          //**Type definitions**************************************************
          using Op = InvokeOverPrimitivesOpBase<AcrossPrimPolicy>;
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor.
           */
          explicit InvokeOverPrimitivesOp(PrimitivesModule* sim,
                                          PrimitivesTraits const&)
              : Op(), module_handle(*sim), for_each_primitive(sim) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy constructor (copy constructed)
           */
          explicit InvokeOverPrimitivesOp(
              std::tuple<PrimitivesModule*, PrimitivesTraits const&>&& args)
              : InvokeOverPrimitivesOp(
                    cpp17::make_from_tuple<InvokeOverPrimitivesOp>(
                        std::move(args))) {}
          //********************************************************************

          //********************************************************************
          /*!\brief Policy destructor.
           */
          ~InvokeOverPrimitivesOp() = default;
          //********************************************************************

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Execute a callable over all types of primitives
           *
           * \param func Callable to be applied over primitive types
           */
          using Op::invoke_over_primitives;
          /*! \endcond */
          //********************************************************************

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Execute a callable over a single type of primitive
           *
           * \param func Callable to be applied over one primitive type
           */
          template <typename F, typename PrimitivesBlock>
          inline auto subset_for_each(
              F do_func, tmpl::list<PrimitivesBlock>) const noexcept -> F {
            // this cannot happen serially, so push resposibility to for_each
            for_each_primitive(std::ref(do_func))(
                module_handle.template primitives<PrimitivesBlock>());
            return do_func;
          }
          /*! \endcond */
          //********************************************************************

          //********************************************************************
          /*! \cond ELASTICA_INTERNAL */
          /*!\brief Execute a callable over multiple, non-exhaustive types of
           * primitives
           *
           * \details
           * Called when >1 type is requested, but not all types.
           *
           * \param func Callable to be applied over primitive types
           */
          template <typename F, typename... PrimitivesBlocks>
          inline auto subset_for_each(
              F func, tmpl::list<PrimitivesBlocks...>) const noexcept -> F {
            // TODO : Whats the performance impact of std::ref, since this
            // is going to be a tight loop?
            (void)cpp17::apply(
                invoke_over_primitives(for_each_primitive(std::ref(func))),
                std::tie(
                    module_handle.template primitives<PrimitivesBlocks>()...));
            return func;
          }
          /*! \endcond */
          //********************************************************************

          //********************************************************************
          /*!\brief Execute a callable over a subset of primitive types
           *
           * \tparam PrimitivesTypeList The subset types of primitives over
           * which to apply the Callable `func` \param func Callable to be
           * applied over a subset of primitive types
           */
          template <typename PrimitivesTypeList, typename F>
          inline decltype(auto) operator()(F&& func) const noexcept {
            return subset_for_each(std::forward<F>(func), PrimitivesTypeList{});
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Execute a callable over all types of primitives
           *
           * \param func Callable to be applied over primitive types
           */
          template <typename F>
          inline auto operator()(F func) const noexcept -> F {
            // TODO : Whats the performance impact of std::ref, since this
            // is going to be a tight loop?
            (void)cpp17::apply(
                invoke_over_primitives(for_each_primitive(std::ref(func))),
                module_handle.primitives().data());
            return func;
          }
          //********************************************************************

          //********************************************************************
          /*!\brief Stream operator
           */
          friend std::ostream& operator<<(std::ostream& os,
                                          InvokeOverPrimitivesOp const& op) {
            return os << update_primitive_algorithm_desc<ForEachPrimPolicy,
                                                         AcrossPrimPolicy>(
                       op.module_handle.n_primitives());
          }
          //********************************************************************

          //**Member variables**************************************************
          //! Handle to the PrimitivesModule
          PrimitivesModule /*const*/& module_handle;
          //! Functor to be applied over individual primitives
          ForEachPrimitiveOp<ForEachPrimPolicy> const for_each_primitive;
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
        struct ApplyOverPrimitives {
          //**Type definitions**************************************************
          //! Type for tagged tuple
          using type = InvokeOverPrimitivesOp<
              typename PrimitivesTraits::IterationPolicy::
                  ExecutionPolicyForEachSystemType,
              typename PrimitivesTraits::IterationPolicy::
                  ExecutionPolicyAcrossSystemTypes>;
          //********************************************************************

          //**Info method*******************************************************
          static std::string metadata(
              PrimitivesModule const& module_handle) noexcept {
            return update_primitive_algorithm_desc<
                typename PrimitivesTraits::IterationPolicy::
                    ExecutionPolicyForEachSystemType,
                typename PrimitivesTraits::IterationPolicy::
                    ExecutionPolicyAcrossSystemTypes>(
                module_handle.n_primitives());
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
        using UpdateSteps = tmpl::list<ApplyOverPrimitives>;
        //@}
        //**********************************************************************
      };
      //************************************************************************

#undef FWD_CAPTURE
#undef FWD

    }  // namespace environments

  }  // namespace interface

}  // namespace elastica