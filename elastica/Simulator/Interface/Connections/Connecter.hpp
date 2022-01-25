//==============================================================================
/*!
//  \file
//  \brief Connecter class to apply connections during runtime
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

#include <cstddef>  // size_t
#include <memory>
#include <utility>  // pair

///
#include "Simulator/Connections/Connection.hpp"
///
#include "Simulator/Interface/Connections/ConnecterBase.hpp"
#include "Simulator/Interface/Connections/ConnectionUUID.hpp"
#include "Simulator/Interface/Connections/Types.hpp"
///
#include "Utilities/Defer.hpp"
#include "Utilities/NumericAt.hpp"
#include "Utilities/NumericRange.hpp"

namespace elastica {

  namespace interface {

    namespace connections {

      //========================================================================
      //
      //  CLASS DEFINITION
      //
      //========================================================================

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Customization for connections deferred to the user
       * \ingroup connections
       *
       * \see Connecter
       */
      struct DeferredToUserPolicy {
        //**Type definitions****************************************************
        //! Type of the index pair
        using IndexPair =
            std::pair<decltype(::elastica::defer), decltype(::elastica::defer)>;
        //**********************************************************************

        //**Constructors and Destructor*****************************************
        /*!\name Constructors and Destructor*/
        //@{
        explicit DeferredToUserPolicy(IndexPair&&) noexcept {};
        DeferredToUserPolicy(DeferredToUserPolicy&&) noexcept = default;
        ~DeferredToUserPolicy() = default;
        //@}
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Customization for regular connections
       * \ingroup connections
       *
       * \see Connecter
       */
      struct EagerPolicy {
        //**Type definitions****************************************************
        //! Type of the index pair
        using IndexPair = std::pair<std::size_t, std::size_t>;
        //**********************************************************************

        //**Constructors and Destructor*****************************************
        /*!\name Constructors and Destructor*/
        //@{
        explicit EagerPolicy(IndexPair p) noexcept : indices_(std::move(p)){};
        EagerPolicy(EagerPolicy&& other) noexcept
            : indices_(std::move(other.indices_)){};
        ~EagerPolicy() = default;
        //@}
        //**********************************************************************

        //**Index functions*****************************************************
        /*!\name Index functions*/
        //@{

        //**********************************************************************
        /*!\brief return the connectivity index of the first system
         */
        constexpr auto first_index() const noexcept { return indices_.first; }
        //**********************************************************************

        //**********************************************************************
        /*!\brief return the connectivity index of the first system
         */
        constexpr auto second_index() const noexcept { return indices_.second; }
        //**********************************************************************

        //@}
        //**********************************************************************

        //**Member variables****************************************************
        //! Index pair to apply connections on
        IndexPair indices_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief A Connection applicator object, specialized for connections
       * whose information is fully known at compile-time
       * \ingroup connections
       *
       * \details
       * Connecter enforces a connection in \elastica on a pair of \elastica
       * systems
       *
       * \tparam ConnectionPolicy Customization for a non-deferred connection
       * \tparam FirstSystem Type of first system being connected
       * \tparam SecondSystem Type of second system being connected
       * \tparam ConcreteConnection connection being applied
       *
       * \see Connection
       */
      template <typename ConnectionPolicy, typename FirstSystem,
                typename SecondSystem, typename ConcreteConnection>
      class Connecter /*final*/ : public ConnecterBase,
                                  protected ConnectionPolicy {
        /*
         * Dev note:
         * Not marked final because testing needs derivation
         */
       public:
        //**Type definitions****************************************************
        //! Type of the first first_system
        using FirstSystemType = FirstSystem;
        //! Type of the second first_system
        using SecondSystemType = SecondSystem;
        //! Type of the connections
        using ConnectionType = ConcreteConnection;
        //! Type of the time
        using ConnecterBase::TimeType;
        //! Type of the index pair
        using typename ConnectionPolicy::IndexPair;
        //**********************************************************************

       protected:
        //**Index functions*****************************************************
        using ConnectionPolicy::first_index;
        using ConnectionPolicy::second_index;
        //**********************************************************************

       public:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The default constructor.
         *
         * \param uuid UUID of connection
         * \param first_sys first_system
         * \param second_sys second_system
         * \param c Connection
         * \param index_pair index pair
         */
        Connecter(ConnectionUUID uuid, FirstSystemType const& first_sys,
                  SecondSystemType const& second_sys, ConnectionType&& c,
                  IndexPair index_pair)
            : ConnecterBase(std::move(uuid)),
              ConnectionPolicy(std::move(index_pair)),
              first_sys_(first_sys),
              second_sys_(second_sys),
              connection_(std::move(c)){};
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        //**********************************************************************
        /*!\brief destructor
         */
        ~Connecter() override = default;
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Interfaces**********************************************************
        /*!\name Interface methods */
        //@{

        //**********************************************************************
        /*!\brief Apply forces due to a connection between systems
         *
         * \param time_v The current time of simulation
         */
        void apply_forces(TimeType time_v) const noexcept final {
          connection_.apply_forces_internal(time_v, first_sys_, first_index(),
                                            second_sys_, second_index());
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply torques due to a connection between systems
         *
         * \param time_v The current time of simulation
         */
        void apply_torques(TimeType time_v) const noexcept final {
          connection_.apply_torques_internal(time_v, first_sys_, first_index(),
                                             second_sys_, second_index());
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets a handle to the stored connection its type erased
         */
        void* get_connection_unsafe() noexcept final { return &connection_; }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! Reference to first system being forced
        mutable FirstSystemType first_sys_;
        //! Reference to second system being forced
        mutable SecondSystemType second_sys_;
        //! Connection object
        ConnectionType connection_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief A Connection applicator object, specialized for DEFERRED
       * connections whose information is fully known at compile-time
       * \ingroup connections
       *
       * \details
       * Connecter enforces a connection in \elastica on a pair of
       * \elastica systems
       *
       * \tparam ConnectionPolicy Customization for a deferred connection
       * \tparam FirstSystem Type of first system being connected
       * \tparam SecondSystem Type of second system being connected
       * \tparam ConcreteConnection connection being applied
       *
       * \see Connection
       */
      template <typename FirstSystem, typename SecondSystem,
                typename ConcreteConnection>
      class Connecter<DeferredToUserPolicy, FirstSystem, SecondSystem,
                      ConcreteConnection>
          /*final*/ : public ConnecterBase, protected DeferredToUserPolicy {
        /*
         * Dev note:
         * Not marked final because testing needs derivation
         */
       public:
        //**Type definitions****************************************************
        //! Type of the first first_system
        using FirstSystemType = FirstSystem;
        //! Type of the second first_system
        using SecondSystemType = SecondSystem;
        //! Type of the connections
        using ConnectionType = ConcreteConnection;
        //! Type of the time
        using ConnecterBase::TimeType;
        //! Type of the index pair
        using typename DeferredToUserPolicy::IndexPair;
        //**********************************************************************

       public:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The default constructor.
         *
         * \param uuid UUID of connection
         * \param first_sys first_system
         * \param second_sys second_system
         * \param c Connection
         * \param index_pair index pair
         */
        Connecter(ConnectionUUID uuid, FirstSystemType const& first_sys,
                  SecondSystemType const& second_sys, ConnectionType&& c,
                  IndexPair index_pair)
            : ConnecterBase(std::move(uuid)),
              DeferredToUserPolicy(std::move(index_pair)),
              first_sys_(first_sys),
              second_sys_(second_sys),
              connection_(std::move(c)){};
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        //**********************************************************************
        /*!\brief destructor
         */
        ~Connecter() override = default;
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Interfaces**********************************************************
        /*!\name Interface methods */
        //@{

        //**********************************************************************
        /*!\brief Apply forces due to a connection between systems
         *
         * \param time_v The current time of simulation
         */
        void apply_forces(TimeType time_v) const noexcept final {
          connection_.apply_forces_internal(time_v, first_sys_, second_sys_);
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply torques due to a connection between systems
         *
         * \param time_v The current time of simulation
         */
        void apply_torques(TimeType time_v) const noexcept final {
          connection_.apply_torques_internal(time_v, first_sys_, second_sys_);
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets a handle to the stored connection its type erased
         */
        void* get_connection_unsafe() noexcept final { return &connection_; }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! Reference to first system being forced
        mutable FirstSystemType first_sys_;
        //! Reference to second system being forced
        mutable SecondSystemType second_sys_;
        //! Connection object
        ConnectionType connection_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief A Connection applicator object, specialized for runtime
       * connections
       * \ingroup connections
       *
       * \details
       * Connecter enforces a connections on two \elastica systems
       *
       * \tparam FirstSystem first_system being forced
       * \tparam ConnectionRegistrars Registered connections
       * \tparam BaseOfIndexSet Base class of index set (which is either a
       * compact set or a range)
       *
       * \note
       * This is a specialization for runtime connections, and so has two
       * additional indirections
       *
       * \see Connection
       */
      template <typename ConnectionPolicy, typename FirstSystem,
                typename SecondSystem, typename ConnectionRegistrars>
      class Connecter<ConnectionPolicy, FirstSystem, SecondSystem,
                      std::unique_ptr<::elastica::connections::detail::
                                          ConnectionBase<ConnectionRegistrars>>>
          /*final*/ : public ConnecterBase, ConnectionPolicy {
        /*
         * Dev note:
         * Not marked final because testing needs derivation
         */
       public:
        //**Type definitions****************************************************
        //! Type of the first system
        using FirstSystemType = FirstSystem;
        //! Type of the second system
        using SecondSystemType = SecondSystem;
        //! Type of the connections
        using ConnectionType =
            std::unique_ptr<::elastica::connections::detail::ConnectionBase<
                ConnectionRegistrars>>;
        //! Type of the time
        using ConnecterBase::TimeType;
        //! Type of the index pair
        using typename ConnectionPolicy::IndexPair;
        //**********************************************************************

       protected:
        //**Index functions*****************************************************
        using ConnectionPolicy::first_index;
        using ConnectionPolicy::second_index;
        //**********************************************************************

       public:
        //**Constructors********************************************************
        /*!\name Constructors */
        //@{

        //**********************************************************************
        /*!\brief The default constructor.
         *
         * \param uuid UUID of connections
         * \param first_sys first_system
         * \param second_sys second_system
         * \param c Connection
         * \param index_set indexset
         */
        Connecter(ConnectionUUID uuid, FirstSystemType const& first_sys,
                  SecondSystemType const& second_sys, ConnectionType c,
                  IndexPair&& index_pair)
            : ConnecterBase(std::move(uuid)),
              ConnectionPolicy(std::move(index_pair)),
              first_sys_(first_sys),
              second_sys_(second_sys),
              connection_(std::move(c)){};
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Destructor**********************************************************
        /*!\name Destructor */
        //@{
        //**********************************************************************
        /*!\brief destructor
         */
        ~Connecter() override = default;
        //**********************************************************************
        //@}
        //**********************************************************************

        //**Interfaces**********************************************************
        /*!\name Interface methods */
        //@{

        //**********************************************************************
        /*!\brief Apply forces due to a connection between systems
         *
         * \param time_v The current time of simulation
         */
        void apply_forces(TimeType time_v) const noexcept final {
          connection_->apply_forces_internal(time_v, first_sys_, first_index(),
                                             second_sys_, second_index());
        }
        //**********************************************************************

        //**********************************************************************
        /*!\brief Apply torques due to a connection between systems
         *
         * \param time_v The current time of simulation
         */
        void apply_torques(TimeType time_v) const noexcept final {
          connection_->apply_torques_internal(time_v, first_sys_, first_index(),
                                              second_sys_, second_index());
        };
        //**********************************************************************

        //**********************************************************************
        /*!\brief Gets a handle to the stored connection its type erased
         */
        void* get_connection_unsafe() noexcept final {
          return connection_.get();
        }
        //**********************************************************************

        //@}
        //**********************************************************************

       private:
        //**Member variables****************************************************
        //! Reference to first system being forced
        mutable FirstSystemType first_sys_;
        //! Reference to second system being forced
        mutable SecondSystemType second_sys_;
        //! Pointer to a runtime connections
        ConnectionType connection_;
        //**********************************************************************
      };
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Make an \elastica connecter in an idiomatic way
       * \ingroup connections
       *
       * \details
       * We use make_connecter() to get a Connecter object which can then be
       * stored to apply connections during runtime
       *
       * \note
       * There is no type checking for the index set here and it takes any type
       * meeting the index set concept
       *
       * \see Connecter
       */
      template <typename FirstSystem, typename SecondSystem,
                typename ConcreteConnection>
      inline auto make_connecter(ConnectionUUID const& id,
                                 FirstSystem const& first_sys,
                                 SecondSystem const& second_sys,
                                 ConcreteConnection&& connection,
                                 std::pair<int, int> index_pair) /* noexcept */
          -> std::unique_ptr<ConnecterBase> {
        return std::unique_ptr<ConnecterBase>(
            new Connecter<EagerPolicy, FirstSystem, SecondSystem,
                          std::decay_t<ConcreteConnection>>(
                id, first_sys, second_sys,
                std::forward<ConcreteConnection>(connection),
                std::move(index_pair)));
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Make an \elastica connecter in an idiomatic way
       * \ingroup connections
       *
       * \details
       * We use make_connecter() to get a Connecter object which can then be
       * stored to apply connections during runtime
       *
       * \note
       * This is a specialization for a deferred connection
       *
       * \see Connecter
       */
      template <typename FirstSystem, typename SecondSystem,
                typename ConcreteConnection>
      inline auto make_connecter(
          ConnectionUUID const& id, FirstSystem const& first_sys,
          SecondSystem const& second_sys, ConcreteConnection&& connection,
          std::pair<decltype(defer), decltype(defer)> index_pair)
          -> std::unique_ptr<ConnecterBase> {
        return std::unique_ptr<ConnecterBase>(
            new Connecter<DeferredToUserPolicy, FirstSystem, SecondSystem,
                          std::decay_t<ConcreteConnection>>(
                id, first_sys, second_sys,
                std::forward<ConcreteConnection>(connection), index_pair));
      }
      /*! \endcond */
      //************************************************************************

      //************************************************************************
      /*! \cond ELASTICA_INTERNAL */
      /*!\brief Make an \elastica connecter in an idiomatic way
       * \ingroup connections
       *
       * \details
       * We use make_connecter() to get a Connecter object which can then be
       * stored to apply connections during runtime
       *
       * \see Connecter
       */
      template <typename FirstSystem, typename SecondSystem,
                typename ConnectionRegistrars>
      inline auto make_connecter(
          ConnectionUUID const& id, FirstSystem const& first_sys,
          SecondSystem const& second_sys,
          std::unique_ptr<::elastica::connections::detail::ConnectionBase<
              ConnectionRegistrars>>
              connection_ptr,
          std::pair<std::size_t, std::size_t> index_pair)
          -> std::unique_ptr<ConnecterBase> {
        return std::unique_ptr<ConnecterBase>(
            new Connecter<
                EagerPolicy, FirstSystem, SecondSystem,
                std::unique_ptr<::elastica::connections::detail::ConnectionBase<
                    ConnectionRegistrars>>>(id, first_sys, second_sys,
                                            std::move(connection_ptr),
                                            std::move(index_pair)));
      }
      /*! \endcond */
      //************************************************************************

    }  // namespace connections

  }  // namespace interface

}  // namespace elastica
