//==============================================================================
/*!
//  \file
//  \brief Interface to add forcings to the \elastica Forcing module
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

#include <exception>
#include <limits>
///
#include "Simulator/Interface/Forcing/Types.hpp"
///
#include "Simulator/Interface/Forcing/Forcer.hpp"
#include "Simulator/Interface/Forcing/ForcingBase.hpp"
#include "Simulator/Interface/Forcing/ForcingUUID.hpp"
///
#include "ErrorHandling/NestedExceptions.hpp"
#include "Simulator/Forcing/Protocols.hpp"
#include "Simulator/Forcing/Types.hpp"
#include "Simulator/Interface/Indexers.hpp"  // to_numeric and TT
#include "Simulator/Interface/Wrappers/Wrappers.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/MakeCopyable.hpp"
#include "Utilities/MakeString.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace elastica {

  namespace interface {

    namespace forcing {

      namespace detail {
        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        //@{

        //**********************************************************************
        /*!\brief Requirements to be met for a Forcing type during
         * compile-time
         */
        template <typename ConcreteForcing>
        struct ConcreteForcingConcept
            : public ::tt::conforms_to<
                  ConcreteForcing, ::elastica::forcing::protocols::Forcing> {};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Specialization of Forcing requirement for runtime support
         */
        template <typename ConcreteForcing, typename Deleter>
        struct ConcreteForcingConcept<std::unique_ptr<ConcreteForcing, Deleter>>
            : public ConcreteForcingConcept<ConcreteForcing> {};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Requirements to be met for a Index type during
         * compile-time
         */
        template <typename IndexSet>
        struct IndexSetConcept
            : public ::std::disjunction<tt::IsAt<IndexSet>,
                                          tt::IsRange<IndexSet>> {};
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
      /*!\brief Temporary memoization of forcing information for \elastica
       *  systems types
       * \ingroup forcing
       *
       * \see forcing
       */
      template <typename SystemRef, typename ForcingRegistrars>
      class Forcing : public ForcingBase<ForcingRegistrars> {
       public:
        //**Type definitions****************************************************
        //! Parent type
        using Parent = ForcingBase<ForcingRegistrars>;
        //! This type
        using This = Forcing<SystemRef, ForcingRegistrars>;
        //! Internal size type
        using SizeType = std::size_t;
        //! Internal type of index
        using IndexType = typename Parent::IndexType;
        //! Type of boundary conditions added
        using ForcerID = typename Parent::ForcerID;
        //**********************************************************************

        /* Dev note : While making the module a friend class seems like a good
         * proposition (as only it is allowed to construct a forcing), this
         * makes testing really hard. So instead we make the constructor
         * public
         */
        //**Friendships*********************************************************
        //! Friend with the main entry-point to the Simulator
        template <class SimTraits,  // Simulator options
                  class Sim>        // Simulator class
        friend class Forcing;
        //**********************************************************************

       public:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{
        //**********************************************************************
        /*!\brief Sole constructor.
         *
         * \param id A unique integral ID to identify the forcing
         * \param s SystemRef to be forced
         * \param name Name of the forcing
         */
        explicit Forcing(IndexType id, SystemRef const& s, std::string&& name)
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

        //**********************************************************************
        /*!\brief Destructor
         */
        ~Forcing() override = default;
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**Metafunctions*******************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\name Meta-programming functions
           Helper functions for metaprogramming forcing interface*/
        //@{

        //**********************************************************************
        /*!\brief Compile-time check for a conformant forcing passed to
         * interface
         *
         * \tparam ConcreteForcing The type of forcing
         *
         * \metareturns std::bool_constant
         */
        template <typename ConcreteForcing>
        static inline constexpr auto conformant_forcing() noexcept {
          return detail::ConcreteForcingConcept<
              std::decay_t<ConcreteForcing>>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a non-conformant forcing passed to
         * interface
         *
         * \tparam ConcreteForcing The type of forcing
         *
         * \metareturns std::bool_constant
         *
         * \see conformant_forcing()
         */
        template <typename ConcreteForcing>
        static inline constexpr auto non_conformant_forcing() noexcept {
          return std::negation<
              decltype(conformant_forcing<ConcreteForcing>())>{};
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
         * \tparam ConcreteForcing The type of forcing
         * \tparam IndexSet The type of index set
         *
         * \metareturns std::bool_constant
         */
        template <typename ConcreteForcing, typename IndexSet>
        static inline constexpr auto conformant_interface() noexcept {
          return std::conjunction<
              decltype(conformant_forcing<ConcreteForcing>()),
              decltype(conformant_index_set<IndexSet>())>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a non-conformant interface expected by
         * the simulator
         *
         * \tparam ConcreteForcing The type of forcing
         * \tparam IndexSet The type of index set
         *
         * \metareturns std::bool_constant
         */
        template <typename ConcreteForcing, typename IndexSet>
        static inline constexpr auto non_conformant_interface() noexcept {
          return std::negation<
              decltype(conformant_interface<ConcreteForcing, IndexSet>())>{};
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
elastica's Forcing module. Currently it accepts only compact set of integers and
a range of indices.
)error");
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Static checks and errors for when an input forcing is
         * non-conformant
         */
        template <typename ConcreteForcing,
                  bool B = conformant_forcing<ConcreteForcing>()>
        static inline constexpr auto check_forcing(
            ConcreteForcing&& /*forcing*/) noexcept -> void {
          static_assert(B,
                        R"error(
The input forcing does not conform to a Forcing expected by elastica.
Please check if you
- are calling make_forcing() incorrectly
- are using ELASTICA_MAKE_FORCING() correctly
- or in case you are writing a custom forcing, did you forget to derive from
forcing::Forcing?
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
        /*!\brief Record name of forcing
         *
         * \details
         * store the static name for debugging in case of a user trying to apply
         * a Forcing partially
         */
        template <typename ConcreteForcing>
        inline void record_name_of() {
          this->set_concrete_forcing_type() = pretty_type::short_name<
              std::remove_reference_t<ConcreteForcing>>();
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Interface implementation method
         *
         * \details
         * This method is the actual workhorse in registering a forcing
         * within \elastica. Because its interface, while intutive, may throw
         * off new users, we have other delegated interfaces calling into this
         * function.
         *
         * \param forcing A valid \elastica forcing object
         * \param indices A set or range of indices
         * \exception std::nested_exception
         *
         * \example
         * \snippet Interface/Forcing/Test_Forcing.cpp main_with_eg
         *
         * \see with(), at()
         *
         * \note
         * Error-handling is not done here, see #with instead.
         */
        template <typename ConcreteForcing, typename IndexSet,
                  Requires<conformant_forcing<ConcreteForcing>() &&
                           conformant_index_set<IndexSet>()> = nullptr>
        void unsafe_with(ConcreteForcing&& forcing,
                         IndexSet&& indices) /*noexcept*/
        {
          using NumericIndexSet = decltype(to_numeric(
              std::forward<IndexSet>(indices), this->system_ref_.get()));

          record_name_of<ConcreteForcing>();

          try {
            struct {
              ConcreteForcing bc;
              NumericIndexSet index_set;
            } capture{std::forward<ConcreteForcing>(forcing),
                      to_numeric(std::forward<IndexSet>(indices),
                                 this->system_ref_.get())};
            Parent::forcer_generator_ = make_copyable_function(
                [this, forcing_details = std::move(capture)]() mutable {
                  /* Developer note:
                   * we directly pass in ConcreteBC<DefaultRegistrars>, but
                   * the intended type is ConcreteBC<ForcingRegistrars>. This
                   * doesn't typically matter for compile-time registration of
                   * BCs
                   */
                  auto unique_id = this->id().uuid;

                  auto unique_name = this->id().name + "of_type_" +
                                     this->concrete_forcing_type() +
                                     "with_system_" +
                                     this->concrete_system_type();

                  return make_forcer(
                      typename Parent::UUIDType{unique_id,
                                                std::move(unique_name)},
                      system_ref_.get(), std::move(forcing_details.bc),
                      std::move(forcing_details.index_set));
                });
          } catch (...) {
            const auto error_message = std::string(
                MakeString{} << "Attempts to force system of type "
                             << this->concrete_system_type() << " with a "
                             << this->concrete_forcing_type() << " failed:");
            std::throw_with_nested(std::runtime_error(error_message));
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Interface implementation
         *
         * \details
         * When the forcing/index-set fails to meet concepts, we publish a
         * compile-time error message via this function.
         *
         * \param forcing A valid \elastica forcing object
         * \param indices A set of indices
         *
         * \see with(), at()
         */
        template <typename ConcreteForcing, typename IndexSet,
                  Requires<non_conformant_interface<ConcreteForcing,
                                                    IndexSet>()> = nullptr>
        static inline constexpr auto unsafe_with(ConcreteForcing&& forcing,
                                                 IndexSet&& indices) noexcept
            -> void {
          check_forcing(std::forward<ConcreteForcing>(forcing));
          check_index_set(std::forward<IndexSet>(indices));
        }
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Main interface method
         *
         * \details
         * This method is the actual workhorse in registering a forcing
         * within \elastica. Because its interface, while intutive, may throw
         * off new users, we have other delegated interfaces calling into this
         * function. This method also handles exceptions.
         *
         * \param forcing A valid \elastica forcing object
         * \param indices A set or range of indices
         * \exception std::out_of_range Indices are out of system's range
         *
         * \example
         * \snippet Interface/Forcing/Test_Forcing.cpp main_with_eg
         *
         * \see with(), at()
         */
        template <typename ConcreteForcing, typename IndexSet>
        decltype(auto) with(ConcreteForcing&& forcing,
                            IndexSet&& indices) /*noexcept*/
        {
          try {
            unsafe_with(std::forward<ConcreteForcing>(forcing),
                        std::forward<IndexSet>(indices));
          } catch (const std::exception& e) {
            abort_with_backtrace(e);
          }

          // return a key for querying after finalizing
          using RT = const ForcingQueryID<std::decay_t<ConcreteForcing>>;
          return RT{this->id()};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Flowable interface helper
         *
         * \details
         * with() method helps in making the frontend interface of \elastica
         * cleaner by allowing to insert forcing anywhere within the
         * actual force() statement.
         *
         * \param forcing A valid \elastica forcing object
         *
         * \example
         * \snippet Interface/Forcing/Test_Forcing.cpp with_eg
         *
         * \note
         * SFINAE check for derived from is taken care in the interior `with`
         * to reduce compilation times at the expense of a more cryptic error
         */
        template <typename ConcreteForcing>
        auto with(ConcreteForcing&& forcing) noexcept {
          record_name_of<ConcreteForcing>();

          struct {
            ConcreteForcing force;
          } capture{std::forward<ConcreteForcing>(forcing)};
          // mark return with decltype auto to give a reference to the current
          // object, and not make a copy
          return make_wrapped_at(
              [this, forcing_details = std::move(capture)](
                  auto&& first_index_or_range,
                  auto&&... indices) mutable -> decltype(auto) {
                return this->with(
                    std::move(forcing_details.force),
                    ::elastica::interface::at(
                        std::forward<decltype(first_index_or_range)>(
                            first_index_or_range),
                        std::forward<decltype(indices)>(indices)...));
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Flowable interface helper
         *
         * \details
         * with() method helps in making the frontend interface of \elastica
         * cleaner by allowing to insert forcing anywhere within the
         * actual force() statement.
         *
         * \param ... forcing_args Arguments to the forcing type
         * \tparam ConcreteForcing A type adhering to \elastica Forcing concept
         *
         * \example
         * \snippet Interface/Forcing/Test_Forcing.cpp templated_with_eg
         *
         * \note
         * SFINAE check for derived from is taken care in the interior `with`
         * to reduce compilation times at the expense of a more cryptic error
         */
        template <
            typename ConcreteForcing, typename... ConcreteForcingArgs,
            Requires<std::is_constructible<
                ConcreteForcing, ConcreteForcingArgs...>::value> = nullptr>
        auto with(ConcreteForcingArgs&&... forcing_args) noexcept {
          record_name_of<ConcreteForcing>();

          // mark return with decltype auto to give a reference to the current
          // object, and not make a copy
          return make_wrapped_at(
              [this, forcing = ConcreteForcing(
                         std::forward<ConcreteForcingArgs>(forcing_args)...)](
                  auto&& first_index_or_range,
                  auto&&... indices) mutable -> decltype(auto) {
                return this->with(
                    std::move(forcing),
                    ::elastica::interface::at(
                        std::forward<decltype(first_index_or_range)>(
                            first_index_or_range),
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
         * force() statement
         *
         * \param first An index/range onto which the system is constrained
         * \param ... indices Other indices onto which the system is constrained
         *
         * \example
         * \snippet Interface/Forcing/Test_Forcing.cpp interface_at_example
         *
         * \note
         * SFINAE check for derived from is taken care in the interior `with`
         * to reduce compilation times at the expense of a more cryptic error
         */
        template <typename IndexOrRange, typename... Indices>
        auto at(IndexOrRange&& first, Indices&&... indices) noexcept {
          // mark return with decltype auto to give a reference to the current
          // object, and not make a copy
          return make_wrapped_with(
              [this, index_set = ::elastica::interface::at(
                         std::forward<IndexOrRange>(first),
                         std::forward<Indices>(indices)...)](
                  auto&& forcing) mutable -> decltype(auto) {
                return this->with(std::forward<decltype(forcing)>(forcing),
                                  std::move(index_set));
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Sets the name of the forcing
         *
         * \param name of the forcing
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
        /*!\brief Sets the name of the forcing
         *
         * \param name of the forcing
         * \return void
         */
        // inline decltype(auto) with_name(std::string name) noexcept {
        //   return set_name(std::move(name));
        // }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Check if the current Forcing has all necessary information
         * for making a forcer
         *
         * \return bool, true if not valid else false
         */
        bool is_not_valid() const noexcept override {
          // are the indices also valid?
          return !Parent::forcer_generator_;
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**********************************************************************
        /*!\brief The finalize method.
         *
         * \details
         * The finalize method is the main interface to get the finalized
         * Forcer that will act on systems within \elastica
         */
        ForcerID finalize() const override {
          /*
           * developer note:
           * In theory, one can store a std::type_info object of a base ptr and
           * compare it to the registered classes to know the dynamic type, but
           * this is probably not what we want given the number of instantations
           * needed
           * n_systems * n_bcs * n_indices > 100!
           */
          return this->forcer_generator_();
        }
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! The type of system being forced
        SystemRef system_ref_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace forcing

  }  // namespace interface

}  // namespace elastica
