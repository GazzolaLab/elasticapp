//==============================================================================
/*!
//  \file
//  \brief Interface to add connections to the \elastica Connections module
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
#include <typeindex>

///
#include "Simulator/Interface/Connections/Types.hpp"
///
#include "Simulator/Interface/Connections/Connecter.hpp"
#include "Simulator/Interface/Connections/ConnectionBase.hpp"
#include "Simulator/Interface/Connections/ConnectionUUID.hpp"
///
#include "ErrorHandling/NestedExceptions.hpp"
#include "Simulator/Connections/Protocols.hpp"
#include "Simulator/Connections/Types.hpp"
#include "Simulator/Interface/Indexers.hpp"
#include "Simulator/Interface/Wrappers/Wrappers.hpp"
//#include "Utilities/Hash.hpp"
#include "Utilities/MakeCopyable.hpp"
#include "Utilities/MakeString.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/ProtocolHelpers.hpp"

namespace elastica {

  namespace interface {

    namespace connections {

      namespace detail {

        //**********************************************************************
        /*! \cond ELASTICA_INTERNAL */
        //@{
        //**********************************************************************
        /*!\brief Requirements to be met for a Connection type during
         * compile-time
         */
        template <typename ConcreteConnection>
        struct ConcreteConnectionConcept
            : public ::tt::conforms_to<
                  ConcreteConnection,
                  ::elastica::connections::protocols::Connection> {};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Specialization of Connection requirement for runtime support
         */
        template <typename ConcreteConnection, typename Deleter>
        struct ConcreteConnectionConcept<
            std::unique_ptr<ConcreteConnection, Deleter>>
            : public ConcreteConnectionConcept<ConcreteConnection> {};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Requirements to be met for a Index-pair type during
         * compile-time
         */
        template <typename IndexPair>
        struct IndexPairConcept : public tt::IsPair<IndexPair> {};
        //**********************************************************************

        //**********************************************************************
        /*!\brief Specialization of IndexPair requirement for runtime support
         */
        template <typename IndexPair, typename Deleter>
        struct IndexPairConcept<std::unique_ptr<IndexPair, Deleter>>
            : public IndexPairConcept<IndexPair> {};
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
      /*!\brief Temporary memoization of connection information for \elastica
       *  systems types
       * \ingroup connections
       *
       * \see ConnectionImpl
       */
      template <typename FirstSystem, typename SecondSystem,
                typename ConnectionRegistrars>
      class Connection : public ConnectionBase<ConnectionRegistrars> {
       public:
        //**Type definitions****************************************************
        //! Parent type
        using Parent = ConnectionBase<ConnectionRegistrars>;
        //! This type
        using This =
            Connection<FirstSystem, SecondSystem, ConnectionRegistrars>;
        //! Internal size type
        using SizeType = std::size_t;
        //! Internal type of index
        using IndexType = typename Parent::IndexType;
        //! Type of boundary conditions added
        using ConnecterID = typename Parent::ConnecterID;
        //**********************************************************************

        /* Dev note : While making the module a friend class seems like a good
         * proposition (as only it is allowed to construct a conn.), this
         * makes testing really hard. So instead we make the constructor
         * public
         */
        //**Friendships*********************************************************
        template <class SimTraits,  // Simulator options
                  class Sim>        // Simulator class
        friend class Connections;
        //**********************************************************************

        // protected:
       public:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief Sole constructor.
         *
         * \param id A unique integral ID to identify the connection
         * \param s1 FirstSystem to be connected
         * \param s2 SecondSystem to be connected
         * \param name Name of the connection
         */
        explicit Connection(IndexType id, FirstSystem const& s1,
                            SecondSystem const& s2, std::string&& name)
            : Parent(id, std::move(name)),
              first_system_(s1),
              second_system_(s2) {
          this->set_first_concrete_system_type() =
              pretty_type::short_name<FirstSystem>();
          this->set_second_concrete_system_type() =
              pretty_type::short_name<SecondSystem>();
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
        ~Connection() override = default;
        //**********************************************************************

        //@}
        //**********************************************************************

       protected:
        //**Metafunctions*******************************************************
        /*! \cond ELASTICA_INTERNAL */
        /*!\name Meta-programming functions
           Helper functions for metaprogramming connection interface*/
        //@{

        //**********************************************************************
        /*!\brief Compile-time check for a conformant connection passed to
         * interface
         *
         * \tparam ConcreteConnection The type of connection
         *
         * \metareturns cpp17::bool_constant
         */
        template <typename ConcreteConnection>
        static inline constexpr auto conformant_connection() noexcept {
          return detail::ConcreteConnectionConcept<
              std::decay_t<ConcreteConnection>>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a non-conformant connection passed to
         * interface
         *
         * \tparam ConcreteConnection The type of connection
         *
         * \metareturns cpp17::bool_constant
         *
         * \see conformant_connection()
         */
        template <typename ConcreteConnection>
        static inline constexpr auto non_conformant_connection() noexcept {
          return cpp17::negation<
              decltype(conformant_connection<ConcreteConnection>())>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a conformant index pair passed to
         * interface
         *
         * \tparam IndexPair The type of index pair
         *
         * \metareturns cpp17::bool_constant
         */
        template <typename IndexPair>
        static inline constexpr auto conformant_index_pair() noexcept {
          return detail::IndexPairConcept<std::decay_t<IndexPair>>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a nonconformant index pair passed to
         * interface
         *
         * \tparam IndexPair The type of index pair
         *
         * \metareturns cpp17::bool_constant
         *
         * \see conformant_index_pair
         */
        template <typename IndexPair>
        static inline constexpr auto non_conformant_index_pair() noexcept {
          return cpp17::negation<
              decltype(conformant_index_pair<IndexPair>())>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a conformant interface expected by
         * the simulator
         *
         * \tparam ConcreteConnection The type of connection
         * \tparam IndexPair The type of index pair
         *
         * \metareturns cpp17::bool_constant
         */
        template <typename ConcreteConnection, typename IndexPair>
        static inline constexpr auto conformant_interface() noexcept {
          return cpp17::conjunction<
              decltype(conformant_connection<ConcreteConnection>()),
              decltype(conformant_index_pair<IndexPair>())>{};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Compile-time check for a non-conformant interface expected by
         * the simulator
         *
         * \tparam ConcreteConnection The type of connection
         * \tparam IndexPair The type of index pair
         *
         * \metareturns cpp17::bool_constant
         */
        template <typename ConcreteConnection, typename IndexPair>
        static inline constexpr auto non_conformant_interface() noexcept {
          return cpp17::negation<decltype(conformant_interface<
                                          ConcreteConnection, IndexPair>())>{};
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
        template <typename IndexPair,
                  bool B = conformant_index_pair<IndexPair>()>
        static inline constexpr auto check_index_pair(
            IndexPair&& /*indices*/) noexcept -> void {
          static_assert(B,
                        R"error(
The pair of indices passed in does not conform to the pair of indices expected by
elastica's Connection module. Currently it accepts only a pair of integer, end
or deferred indices.
)error");
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Static checks and errors for when an input connection is
         * non-conformant
         */
        template <typename ConcreteConnection,
                  bool B = conformant_connection<ConcreteConnection>()>
        static inline constexpr auto check_connection(
            ConcreteConnection&& /*connection*/) noexcept -> void {
          static_assert(B,
                        R"error(
The input connection does not conform to a Connection expected by elastica.
Please check if you
- are calling make_connection() incorrectly
- are using ELASTICA_MAKE_CONNECTION() correctly
- or in case you are writing a custom connection, did you forget to derive from
connections::Connection?
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
        /*!\brief Record name of connection
         *
         * \details
         * store the static name for debugging in case of a user trying to apply
         * a Connection partially
         */
        template <typename ConcreteConnection>
        inline void record_name_of() {
          this->set_concrete_connection_type() = pretty_type::short_name<
              std::remove_reference_t<ConcreteConnection>>();
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Interface implementation method
         *
         * \details
         * This method is the actual workhorse in registering a connection
         * within \elastica. Because its interface, while intutive, may throw
         * off new users, we have other delegated interfaces calling into this
         * function.
         *
         * \param connection A valid \elastica connection object
         * \param indices A set of indices
         * \exception std::nested_exception
         *
         * \see with(), at()
         *
         * \note
         * Error-handling is not done here, see #with instead.
         */
        template <typename ConcreteConnection, typename IndexPair,
                  Requires<conformant_interface<ConcreteConnection,
                                                IndexPair>()> = nullptr>
        void unsafe_with(ConcreteConnection&& connection,
                         IndexPair&& index_pair) /*noexcept*/ {
          // https://stackoverflow.com/questions/26831382/capturing-perfectly-forwarded-variable-in-lambda
          // https://vittorioromeo.info/index/blog/capturing_perfectly_forwarded_objects_in_lambdas.html
          using NumericIndexPair =
              decltype(to_numeric(std::forward<IndexPair>(index_pair),
                                  this->first_system_, this->second_system_));

          record_name_of<ConcreteConnection>();

          try {
            struct {
              ConcreteConnection connection;
              NumericIndexPair index_pair;
            } capture{std::forward<ConcreteConnection>(connection),
                      to_numeric(std::forward<IndexPair>(index_pair),
                                 this->first_system_, this->second_system_)};
            // we cannot directly assign the lambda to a std::function because
            // it us not copyable and the requirements of std::function need a
            // copyable callable so we work around it
            // https://stackoverflow.com/a/20846873
            Parent::connecter_generator_ = make_copyable_function(
                [this, connection_details = std::move(capture)]() mutable {
                  // Look ma, no type erasure!
                  // TODO id() is copied, but it's not that costly
                  /* Developer note:
                   * we directly pass in ConcreteBC<DefaultRegistrars>, but
                   * the intended type is ConcreteBC<ConnectionRegistrars>.
                   * This doesn't typically matter for compile-time
                   * registration of BCs
                   */

                  // typeindex is not guaranteed to return the same results
                  // in between two runs, so skip hash_combine ...
                  auto unique_id = this->id().uuid;
                  /*
                   * hash_combine(unique_id,
                   *              std::type_index(typeid(FirstSystem)));
                   * hash_combine(unique_id,
                   *              std::type_index(typeid(SecondSystem)));
                   * hash_combine(unique_id,
                   *              std::type_index(typeid(ConcreteConnection)));
                   */

                  // ... but modify the name a bit so that when we load/store we
                  // can confirm integrity of data
                  auto unique_name = this->id().name + "of_type_" +
                                     this->concrete_connection_type() +
                                     "with_first_system_" +
                                     this->first_concrete_system_type() +
                                     "with_second_system_" +
                                     this->second_concrete_system_type();
                  return make_connecter(
                      typename Parent::UUIDType{unique_id,
                                                std::move(unique_name)},
                      // while const-correctness is maintained in Connection,
                      // we need to modify the systems later on, so cast it
                      // away
                      const_cast<FirstSystem&>(first_system_),
                      const_cast<SecondSystem&>(second_system_),
                      std::move(connection_details.connection),
                      std::move(connection_details.index_pair));
                });
          } catch (...) {
            const auto error_message =
                std::string(MakeString{}
                            << "Attempts to connect systems of type "
                            << this->first_concrete_system_type() << " and "
                            << this->second_concrete_system_type() << " with a "
                            << this->concrete_connection_type() << " failed:");
            std::throw_with_nested(std::runtime_error(error_message));
          }
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Interface implementation method
         *
         * \details
         * When the connection/index-pair fails to meet concepts, we publish a
         * compile-time error message via this function.
         *
         * \param connection A valid \elastica connection object
         * \param indices A pair of indices
         *
         * \see with(), at()
         */
        template <typename ConcreteConnection, typename IndexPair,
                  Requires<non_conformant_interface<ConcreteConnection,
                                                    IndexPair>()> = nullptr>
        static inline constexpr void unsafe_with(
            ConcreteConnection&& connection, IndexPair&& indices) noexcept {
          check_connection(std::forward<ConcreteConnection>(connection));
          check_index_pair(std::forward<IndexPair>(indices));
        }
        //**********************************************************************

       public:
        //**********************************************************************
        /*!\brief Main interface method
         *
         * \details
         * This method is the actual workhorse in registering a connection
         * within \elastica. Because its interface, while intutive, may throw
         * off new users, we have other delegated interfaces calling into this
         * function.
         *
         * \param connection A valid \elastica connection object
         * \param indices A set of indices
         *
         * \example
         * \snippet Interface/Connections/Test_Connection.cpp main_with_eg
         *
         * \see #unsafe_with() for implementation details
         */
        template <typename ConcreteConnection, typename IndexPair>
        auto with(ConcreteConnection&& connection,
                  IndexPair&& index_pair) /*noexcept*/
        {
          try {
            unsafe_with(std::forward<ConcreteConnection>(connection),
                        std::forward<IndexPair>(index_pair));
          } catch (const std::exception& e) {
            abort_with_backtrace(e);
          }

          // retrun a key for querying after finalizing
          using ::elastica::interface::detail::id;
          return ConnectionQueryID<std::decay_t<ConcreteConnection>>{
              id(this->id())};
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Flowable interface helper
         *
         * \details
         * with() method helps in making the frontend interface of \elastica
         * cleaner by allowing to insert connections anywhere within the
         * actual connect() statement.
         *
         * \param connection A valid \elastica connection object
         *
         * \example
         * \snippet Interface/Connections/Test_Connection.cpp with_eg
         *
         * \note
         * SFINAE check for derived from is taken care in the interior `with`
         * to reduce compilation times at the expense of a more cryptic error
         */
        template <typename ConcreteConnection>
        auto with(ConcreteConnection&& connection) noexcept {
          record_name_of<ConcreteConnection>();

          struct {
            ConcreteConnection bc;
          } capture{std::forward<ConcreteConnection>(connection)};
          // mark return with decltype auto to give a reference to the current
          // object, and not make a copy
          return make_wrapped_at(
              [this, connection_details = std::move(capture)](
                  auto&& index_pair) mutable -> decltype(auto) {
                return this->with(
                    std::move(connection_details.bc),
                    std::forward<decltype(index_pair)>(index_pair));
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Flowable interface helper
         *
         * \details
         * with() method helps in making the frontend interface of \elastica
         * cleaner by allowing to insert connections anywhere within the
         * actual connect() statement.
         *
         * \param connection_args Arguments to the connection type
         * \tparam ConcreteConnection A type adhering to \elastica Connection
         * concept
         *
         * \example
         * \snippet Interface/Connections/Test_Connection.cpp templated_with_eg
         *
         * \note
         * SFINAE check for derived from is taken care in the interior `with`
         * to reduce compilation times at the expense of a more cryptic error
         */
        template <typename ConcreteConnection,
                  typename... ConcreteConnectionArgs,
                  Requires<std::is_constructible<
                      ConcreteConnection, ConcreteConnectionArgs...>::value> =
                      nullptr>
        auto with(ConcreteConnectionArgs&&... connection_args) noexcept {
          record_name_of<ConcreteConnection>();

          // mark return with decltype auto to give a reference to the current
          // object, and not make a copy
          return make_wrapped_at(
              [this,
               connection = ConcreteConnection(
                   std::forward<ConcreteConnectionArgs>(connection_args)...)](
                  auto&& index_pair) mutable -> decltype(auto) {
                return this->with(
                    std::move(connection),
                    std::forward<decltype(index_pair)>(index_pair));
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Flowable interface helper
         *
         * \details
         * at() method helps in making the frontend interface of \elastica
         * cleaner by allowing to insert indices anywhere within the actual
         * connect() statement
         *
         * \param index_pair A pair of indices onto which the systems are
         * connected
         *
         * \example
         * \snippet Interface/Connections/Test_Connection.cpp at_eg
         */
        template <typename FirstIndex, typename SecondIndex>
        auto at(::elastica::interface::detail::Pair_t<FirstIndex, SecondIndex>&&
                    index_pair) noexcept {
          // mark return with decltype auto to give a reference to the current
          // object, and not make a copy
          return make_wrapped_with(
              [this, index_pair = std::move(index_pair)](
                  auto&& connection) mutable -> decltype(auto) {
                return this->with(
                    std::forward<decltype(connection)>(connection),
                    std::move(index_pair));
              });
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Sets the name of the connection
         *
         * \param name of the connection
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
        /*!\brief Sets the name of the connection
         *
         * \param name of the connection
         * \return void
         */
        inline decltype(auto) with_name(std::string name) noexcept {
          return set_name(std::move(name));
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Check if the current Connection has all necessary information
         * for making a connecter
         *
         * \return bool, true if not valid else false
         */
        bool is_not_valid() const noexcept override {
          // are the indices also valid?
          return !Parent::connecter_generator_;
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
         * Connecter that will act on systems within \elastica
         */
        ConnecterID finalize() const override {
          /*
           * developer note:
           * In theory, one can store a std::type_info object of a base ptr and
           * compare it to the registered classes to know the dynamic type, but
           * this is probably not what we want given the number of instantations
           * needed
           * n_systems * n_bcs * n_indices > 100!
           */
          return this->connecter_generator_();
        }
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! The first system being connected
        FirstSystem const& first_system_;
        //! The second system being connected
        SecondSystem const& second_system_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

    }  // namespace connections

  }  // namespace interface

}  // namespace elastica
