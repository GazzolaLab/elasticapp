//==============================================================================
/*!
//  \file
//  \brief Interface to add constraints to the \elastica Constraints module
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

#include <exception>
#include <limits>
///
#include "Simulator/Interface/Constraints/Types.hpp"
///
#include "Simulator/Interface/Constraints/Constrainer.hpp"
#include "Simulator/Interface/Constraints/ConstraintBase.hpp"
#include "Simulator/Interface/Constraints/ConstraintUUID.hpp"
///
#include "ErrorHandling/NestedExceptions.hpp"
#include "Simulator/Constraints/Protocols.hpp"
#include "Simulator/Constraints/Types.hpp"
#include "Simulator/Interface/Indexers.hpp"  // to_numeric and TT
#include "Simulator/Interface/Wrappers/Wrappers.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/MakeCopyable.hpp"
#include "Utilities/MakeString.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace interface {

    namespace constraints {

      namespace detail {

        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        //@{
        //**********************************************************************
        /*!\brief Requirements to be met for a Constraint type during
         * compile-time
         */
        template <typename ConcreteConstraint>
        struct ConcreteConstraintConcept
            : public ::tt::conforms_to<
                  ConcreteConstraint,
                  ::elastica::constraints::protocols::Constraint> {};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Specialization of Constraint requirement for runtime support
         */
        template <typename ConcreteConstraint, typename Deleter>
        struct ConcreteConstraintConcept<
            std::unique_ptr<ConcreteConstraint, Deleter>>
            : public ConcreteConstraintConcept<ConcreteConstraint> {};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Requirements to be met for a Index type during
         * compile-time
         */
        template <typename IndexSet>
        struct IndexSetConcept : public tt::IsAt<IndexSet> {};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Specialization of IndexSet requirement for runtime support
         */
        template <typename IndexSet, typename Deleter>
        struct IndexSetConcept<std::unique_ptr<IndexSet, Deleter>>
            : public IndexSetConcept<IndexSet> {};
        //**********************************************************************
        //@}
        /*! \endcond */
        //**********************************************************************
      }  // namespace detail

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Temporary memoization of constraint information for \elastica
       *  systems types
       * \ingroup constraints
       *
       * \see ConstraintImpl
       */
      template <typename SystemRef, typename ConstraintRegistrars>
      class Constraint : public ConstraintBase<ConstraintRegistrars> {
       public:
        //**Type definitions****************************************************
        //! Parent type
        using Parent = ConstraintBase<ConstraintRegistrars>;
        //! This type
        using This = Constraint<SystemRef, ConstraintRegistrars>;
        //! Internal size type
        using SizeType = std::size_t;
        //! Internal type of index
        using IndexType = typename Parent::IndexType;
        //! Type of boundary conditions added
        using ConstrainerID = typename Parent::ConstrainerID;
        //**********************************************************************

        /* Dev note : While making the module a friend class seems like a good
         * proposition (as only it is allowed to construct a constraint), this
         * makes testing really hard. So instead we make the constructor
         * public
         */
        //**Friendships*********************************************************
        //! Friend with the main entry-point to the Simulator
        template <class SimTraits,  // Simulator options
                  class Sim>        // Simulator class
        friend class Constraints;
        //**********************************************************************

       public:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{
        //**********************************************************************
        /*!\brief Sole constructor.
         *
         * \param id A unique integral ID to identify the constraint
         * \param s SystemRef to be constrained
         * \param name Name of the constraint
         */
        explicit Constraint(IndexType id, SystemRef const& s,
                            std::string&& name)
            : Parent(id, std::move(name)), system_ref_(s) {
          using SystemRefType = typename SystemRef::type;
          this->set_concrete_system_type() =
              pretty_type::short_name<SystemRefType>();
        };
        //**********************************************************************
        //@}
        //**********************************************************************

       public:
        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{

        //**********************************************************************
        /*!\brief Destructor
         */
        ~Constraint() override = default;
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**Metafunctions*******************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\name Meta-programming functions
           Helper functions for metaprogramming constraint interface*/
        //@{

        //**********************************************************************
        /*!\brief Compile-time check for a conformant constraint passed to
         * interface
         *
         * \tparam ConcreteConstraint The type of constraint
         *
         * \metareturns std::bool_constant
         */
        template <typename ConcreteConstraint>
        static inline constexpr auto conformant_constraint() noexcept {
          return detail::ConcreteConstraintConcept<
              std::decay_t<ConcreteConstraint>>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a non-conformant constraint passed to
         * interface
         *
         * \tparam ConcreteConstraint The type of constraint
         *
         * \metareturns std::bool_constant
         *
         * \see conformant_constraint()
         */
        template <typename ConcreteConstraint>
        static inline constexpr auto non_conformant_constraint() noexcept {
          return std::negation<
              decltype(conformant_constraint<ConcreteConstraint>())>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a conformant index set passed to
         * interface
         *
         * \tparam IndexSet The type of index set
         *
         * \metareturns std::bool_constant
         */
        template <typename IndexSet>
        static inline constexpr auto conformant_index_set() noexcept {
          return detail::IndexSetConcept<std::decay_t<IndexSet>>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a nonconformant index set passed to
         * interface
         *
         * \tparam IndexSet The type of index set
         *
         * \metareturns std::bool_constant
         *
         * \see conformant_index_set
         */
        template <typename IndexSet>
        static inline constexpr auto non_conformant_index_set() noexcept {
          return std::negation<decltype(conformant_index_set<IndexSet>())>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a conformant interface expected by
         * the simulator
         *
         * \tparam ConcreteConstraint The type of constraint
         * \tparam IndexSet The type of index set
         *
         * \metareturns std::bool_constant
         */
        template <typename ConcreteConstraint, typename IndexSet>
        static inline constexpr auto conformant_interface() noexcept {
          return std::conjunction<
              decltype(conformant_constraint<ConcreteConstraint>()),
              decltype(conformant_index_set<IndexSet>())>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a non-conformant interface expected by
         * the simulator
         *
         * \tparam ConcreteConstraint The type of constraint
         * \tparam IndexSet The type of index set
         *
         * \metareturns std::bool_constant
         */
        template <typename ConcreteConstraint, typename IndexSet>
        static inline constexpr auto non_conformant_interface() noexcept {
          return std::negation<
              decltype(conformant_interface<ConcreteConstraint, IndexSet>())>{};
        }
        //**********************************************************************

        //@}
        /*! \endcond */
        //**********************************************************************

       private:
        //**Static checks*******************************************************
        /*!\name static checks and contextual error messages */
        //@{

        //**********************************************************************
        /*!\brief Static checks and errors for when an input index set is
         * non-conformant
         */
        template <typename IndexSet, bool B = conformant_index_set<IndexSet>()>
        static inline constexpr auto check_index_set(
            IndexSet&& /*indices*/) noexcept -> void {
          static_assert(B,
                        R"error(
The set of indices passed in does not conform to the set of indices expected by
elastica's Constraint module. Currently it accepts only compact set of integers.
)error");
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Static checks and errors for when an input constraint is
         * non-conformant
         */
        template <typename ConcreteConstraint,
                  bool B = conformant_constraint<ConcreteConstraint>()>
        static inline constexpr auto check_constraint(
            ConcreteConstraint&& /*constraint*/) noexcept -> void {
          static_assert(B,
                        R"error(
The input constraint does not conform to a Constraint expected by elastica.
Please check if you
- are calling make_constraint() incorrectly
- are using ELASTICA_MAKE_CONSTRAINT() correctly
- or in case you are writing a custom constraint, did you forget to derive from
constraints::Constraint?
)error");
        }
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Interfaces**********************************************************
        /*!\name Interface methods */
        //@{
       protected:
        //**********************************************************************
        /*!\brief Record name of constraint
         *
         * \details
         * store the static name for debugging in case of a user trying to apply
         * a Constraint partially
         */
        template <typename ConcreteConstraint>
        inline void record_name_of() {
          this->set_concrete_constraint_type() = pretty_type::short_name<
              std::remove_reference_t<ConcreteConstraint>>();
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Interface implementation method
         *
         * \details
         * This method is the actual workhorse in registering a constraint
         * within \elastica. Because its interface, while intutive, may throw
         * off new users, we have other delegated interfaces calling into this
         * function.
         *
         * \param constraint A valid \elastica constraint object
         * \param indices A set of indices
         * \exception std::nested_exception
         *
         * \example
         * \snippet Interface/Constraints/Test_Constraint.cpp main_with_eg
         *
         * \see with(), at()
         *
         * \note
         * Error-handling is not done here, see #with instead.
         */
        template <typename ConcreteConstraint, typename IndexSet,
                  Requires<conformant_interface<ConcreteConstraint,
                                                IndexSet>()> = nullptr>
        void unsafe_with(ConcreteConstraint&& constraint,
                         IndexSet&& indices) /*noexcept*/ {
          using NumericIndexSet = decltype(to_numeric(
              std::forward<IndexSet>(indices), this->system_ref_.get()));

          record_name_of<ConcreteConstraint>();

          try {
            // https://stackoverflow.com/questions/26831382/capturing-perfectly-forwarded-variable-in-lambda
            // https://vittorioromeo.info/index/blog/capturing_perfectly_forwarded_objects_in_lambdas.html
            struct {
              ConcreteConstraint bc;
              NumericIndexSet index_set;
            } capture{
                std::forward<ConcreteConstraint>(constraint),
                sort_and_check_duplicates(to_numeric(
                    std::forward<IndexSet>(indices), this->system_ref_.get()))};
            // we cannot directly assign the lambda to a std::function because
            // it us not copyable and the requirements of std::function need a
            // copyable callable so we work around it
            // https://stackoverflow.com/a/20846873
            Parent::constrainer_generator_ = make_copyable_function(
                [this, constraint_details = std::move(capture)]() mutable {
                  // Look ma, no type erasure!
                  // TODO id() is copied, but it's not that costly
                  /* Developer note:
                   * we directly pass in ConcreteBC<DefaultRegistrars>, but
                   * the intended type is ConcreteBC<ConstraintRegistrars>.
                   * This doesn't typically matter for compile-time
                   * registration of BCs
                   */
                  auto unique_id = this->id().uuid;
                  // modify the name a bit so that when we load/store we can
                  // confirm integrity of data
                  auto unique_name = this->id().name + "of_type_" +
                                     this->concrete_constraint_type() +
                                     "with_system_" +
                                     this->concrete_system_type();
                  return make_constrainer(
                      typename Parent::UUIDType{unique_id,
                                                std::move(unique_name)},
                      system_ref_.get(), std::move(constraint_details.bc),
                      std::move(constraint_details.index_set));
                });
          } catch (...) {
            const auto error_message = std::string(
                MakeString{} << "Attempts to constrain system of type "
                             << this->concrete_system_type() << " with a "
                             << this->concrete_constraint_type() << " failed:");
            std::throw_with_nested(std::runtime_error(error_message));
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Interface implementation method
         *
         * \details
         * When the constraint/index-set fails to meet concepts, we publish a
         * compile-time error message via this function.
         *
         * \param constraint A valid \elastica constraint object
         * \param indices A set of indices
         *
         * \see with(), at()
         */
        template <typename ConcreteConstraint, typename IndexSet,
                  Requires<non_conformant_interface<ConcreteConstraint,
                                                    IndexSet>()> = nullptr>
        static inline constexpr void unsafe_with(
            ConcreteConstraint&& constraint, IndexSet&& indices) noexcept {
          check_constraint(std::forward<ConcreteConstraint>(constraint));
          check_index_set(std::forward<IndexSet>(indices));
        }
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Main interface method
         *
         * \details
         * This method is the actual workhorse in registering a constraint
         * within \elastica. Because its interface, while intutive, may throw
         * off new users, we have other delegated interfaces calling into this
         * function. This method also handles exceptions.
         *
         * \param constraint A valid \elastica constraint object
         * \param indices A set of indices
         *
         * \example
         * \snippet Interface/Constraints/Test_Constraint.cpp main_with_eg
         *
         * \see #unsafe_with() for implementation details
         */
        template <typename ConcreteConstraint, typename IndexSet>
        decltype(auto) with(ConcreteConstraint&& constraint,
                            IndexSet&& indices) /*noexcept*/
        {
          try {
            unsafe_with(std::forward<ConcreteConstraint>(constraint),
                        std::forward<IndexSet>(indices));
          } catch (const std::exception& e) {
            abort_with_backtrace(e);
          }

          // return a key for querying after finalizing
          using ::elastica::interface::detail::id;
          using RT = const ConstraintQueryID<std::decay_t<ConcreteConstraint>>;
          return RT{this->id()};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Flowable interface helper
         *
         * \details
         * with() method helps in making the frontend interface of \elastica
         * cleaner by allowing to insert constraints anywhere within the
         * actual constrain() statement.
         *
         * \param constraint A valid \elastica constraint object
         *
         * \example
         * \snippet Interface/Constraints/Test_Constraint.cpp with_eg
         *
         * \note
         * SFINAE check for derived from is taken care in the interior `with`
         * to reduce compilation times at the expense of a more cryptic error
         */
        template <typename ConcreteConstraint>
        auto with(ConcreteConstraint&& constraint) noexcept {
          record_name_of<ConcreteConstraint>();

          struct {
            ConcreteConstraint bc;
          } capture{std::forward<ConcreteConstraint>(constraint)};
          // mark return with decltype auto to give a reference to the current
          // object, and not make a copy
          return make_wrapped_at(
              [this, constraint_details = std::move(capture)](
                  auto&& first_idx,
                  auto&&... indices) mutable -> decltype(auto) {
                return this->with(
                    std::move(constraint_details.bc),
                    ::elastica::interface::at(
                        std::forward<decltype(first_idx)>(first_idx),
                        std::forward<decltype(indices)>(indices)...));
                //                    std::forward<decltype(index_set)>(index_set));
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Flowable interface helper
         *
         * \details
         * with() method helps in making the frontend interface of \elastica
         * cleaner by allowing to insert constraints anywhere within the
         * actual constrain() statement.
         *
         * \param constraint_args Arguments to the constraint type
         * \tparam ConcreteConstraint A type adhering to \elastica Constraint
         * concept
         *
         * \example
         * \snippet Interface/Constraints/Test_Constraint.cpp templated_with_eg
         *
         * \note
         * SFINAE check for derived from is taken care in the interior `with`
         * to reduce compilation times at the expense of a more cryptic error
         */
        template <typename ConcreteConstraint,
                  typename... ConcreteConstraintArgs,
                  Requires<std::is_constructible<
                      ConcreteConstraint, ConcreteConstraintArgs...>::value> =
                      nullptr>
        auto with(ConcreteConstraintArgs&&... constraint_args) noexcept {
          // store the static name for debugging
          record_name_of<ConcreteConstraint>();

          // mark return with decltype auto to give a reference to the current
          // object, and not make a copy
          return make_wrapped_at(
              [this,
               constraint = ConcreteConstraint(
                   std::forward<ConcreteConstraintArgs>(constraint_args)...)](
                  auto&& first_idx,
                  auto&&... indices) mutable -> decltype(auto) {
                return this->with(
                    std::move(constraint),
                    ::elastica::interface::at(
                        std::forward<decltype(first_idx)>(first_idx),
                        std::forward<decltype(indices)>(indices)...));
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Flowable interface helper
         *
         * \details
         * at() method helps in making the frontend interface of \elastica
         * cleaner by allowing to insert indices anywhere within the actual
         * constrain() statement
         *
         * \param first_idx An index onto which the system is constrained
         * \param ... indices Other indices onto which the system is constrained
         *
         * \example
         * \snippet Interface/Constraints/Test_Constraint.cpp at_eg
         *
         * \note
         * SFINAE check for derived from is taken care in the interior `with`
         * to reduce compilation times at the expense of a more cryptic error
         */
        template <typename Index, typename... Indices>
        auto at(Index&& first_idx, Indices&&... indices) noexcept {
          // mark return with decltype auto to give a reference to the current
          // object, and not make a copy
          return make_wrapped_with(
              [this, index_set = ::elastica::interface::at(
                         std::forward<Index>(first_idx),
                         std::forward<Indices>(indices)...)](
                  auto&& constraint) mutable -> decltype(auto) {
                return this->with(
                    std::forward<decltype(constraint)>(constraint),
                    std::move(index_set));
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Sets the name of the constraint
         *
         * \param name of the constraint
         * \return void
         */
        inline This& set_name(std::string name) noexcept {
          /*
           * Dev note:
           * 1. noexcept because of copy-and-swap
           * 2. This takes in an argument because this is exposed as part of the
           * interface
           */
          Parent::set_name() = std::move(name);
          return *this;
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Sets the name of the constraint
         *
         * \param name of the constraint
         * \return void
         */
        // inline decltype(auto) with_name(std::string name) noexcept {
        //   return set_name(std::move(name));
        // }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Check if the current Constraint has all necessary information
         * for making a constrainer
         *
         * \return bool, true if not valid else false
         */
        bool is_not_valid() const noexcept override {
          // are the indices also valid?
          return !Parent::constrainer_generator_;
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief The finalize method.
         *
         * \details
         * The finalize method is the main interface to get the finalized
         * Constrainer that will act on systems within \elastica
         */
        ConstrainerID finalize() const override {
          /*
           * developer note:
           * In theory, one can store a std::type_info object of a base ptr and
           * compare it to the registered classes to know the dynamic type, but
           * this is probably not what we want given the number of instantations
           * needed
           * n_systems * n_bcs * n_indices > 100!
           */
          return this->constrainer_generator_();
        }
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! The type of system being constrained
        SystemRef system_ref_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace constraints

  }  // namespace interface

}  // namespace elastica
