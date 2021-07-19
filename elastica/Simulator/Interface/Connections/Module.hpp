//==============================================================================
/*!
//  \file
//  \brief Implementation of module for Connections
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
#include <exception>  // nested
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

/// All connections types
#include "Simulator/Connections/Types.hpp"
#include "Simulator/Interface/Connections/Types.hpp"
/// Interface methods
#include "Simulator/Interface/Connections/Configuration.hpp"
#include "Simulator/Interface/Connections/Connecter.hpp"
#include "Simulator/Interface/Connections/ConnecterBase.hpp"
#include "Simulator/Interface/Connections/Connection.hpp"
#include "Simulator/Interface/Connections/ConnectionBase.hpp"
#include "Simulator/Interface/Connections/ConnectionUUID.hpp"
#include "Simulator/Interface/Connections/ExecutionPolicy.hpp"
#include "Simulator/Interface/Protocols/Connections.hpp"
/// Wrapped types and indexers
#include "Simulator/Interface/Indexers/At.hpp"
#include "Simulator/Interface/Wrappers/Wrappers.hpp"
/// Serialization support
#include "Simulator/IO/Serialization/Types.hpp"
///
#include "ErrorHandling/NestedExceptions.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/Logging.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/Requires.hpp"

namespace elastica {

  namespace modules {

    //==========================================================================
    //
    //  CLASS DEFINITION
    //
    //==========================================================================

    //**************************************************************************
    /*!\brief The connections module
     * \ingroup modules connections
     *
     * The Connections module is an enhancement to the \elastica Simulator
     * and is responsible for connectivity (joints) linking two \elastica
     * entities (such as rods, rigid bodies, etc.).
     *
     * \tparam SimTraits Options for configuration of the Simulator
     * \tparam Sim Simulator
     *
     * \usage
     * To use Connections, pass it as template argument to
     * ::elastica::make_simulator() as shown below. This needs to be accompanied
     * by an instantiation of
     * ::elastica::configuration::ConnectionsConfiguration as an argument to
     * ::elastica::make_simulator().
     *
     * \code
     * auto connections_configuration =
     * ::elastica::configuration::DefaultConnectionsConfiguration{};
     *
     * auto sim =
     * ::elastica::make_simulator<::elastica::modules::Connections>(
     * ::elastica::configuration::DefaultSystemsConfiguration{},
     * ::elastica::configuration::DefaultDomainConfiguration{},
     *  connections_configuration
     * );
     * \endcode
     * And the simulator `sim` now has capabilities of connecting systems. The
     * sole entry point of connections is the #connect method for which we show
     * examples below.
     *
     * \example
     * For the setup of systems shown below...
     * \snippet Connections/Test_Module.cpp connect_setup
     * ...one can launch a connection between systems in one of many ways, each
     * intended to promote readability and be as clear about the intent as
     * possible, as shown below:
     * \snippet Connections/Test_Module.cpp connect_eg
     * \snippet Connections/Test_Module.cpp connect_eg_curry
     * \snippet Connections/Test_Module.cpp connect_eg_no_curry
     * \snippet Connections/Test_Module.cpp connect_to_eg
     * \snippet Connections/Test_Module.cpp connect_to_eg_curry
     * \snippet Connections/Test_Module.cpp connect_to_eg_no_curry
     * \snippet Connections/Test_Module.cpp connect_idx_eg
     * \snippet Connections/Test_Module.cpp connect_idx_eg_curry
     * \snippet Connections/Test_Module.cpp connect_end_idx_eg
     * \snippet Connections/Test_Module.cpp connect_end_idx_eg_curry
     * \snippet Connections/Test_Module.cpp connect_at_idx_eg
     * \snippet Connections/Test_Module.cpp connect_at_idx_eg_curry
     * \snippet Connections/Test_Module.cpp connect_at_end_idx_eg
     * \snippet Connections/Test_Module.cpp connect_at_end_idx_eg_curry
     *
     * \see ::elastica::make_simulator()
     */
    template <class SimTraits, class Sim>
    class Connections
        : public CRTPHelper<Sim, Connections>,
          public ::tt::ConformsTo<
              ::elastica::interface::protocols::Connections>,
          public NonCopyable,
          private detail::HasConnectionsConfiguration<SimTraits>,
          protected ::elastica::interface::connections::ExecutionPolicy<
              SimTraits, Connections<SimTraits, Sim>> {
     private:
      //**Compile-time checks***************************************************
      //
      //************************************************************************

      //**Type definitions******************************************************
      //! This type
      using This = Connections<SimTraits, Sim>;
      //! CRTP Type
      using CRTP = CRTPHelper<Sim, Connections>;
      //! CRTP methods
      using CRTP::self;
      //! Internal size type
      using SizeType = typename SimTraits::SizeType;
      //! Internal type of index
      using IndexType = typename SimTraits::IndexType;
      //! Algorithm type
      using Algorithm =
          ::elastica::interface::connections::ExecutionPolicy<SimTraits, This>;
      //************************************************************************

     public:
      //**Type definitions******************************************************
      //! Type of simulation time
      using TimeType = typename SimTraits::TimeType;  // public for protocols
      //! ID Type of connection applicator
      using ConnecterID =
          std::unique_ptr<::elastica::interface::connections::ConnecterBase>;
      //! Type of connection registrars
      using ConnectionsRegistrars = typename SimTraits::ConnectionsRegistrars;
      //! ID Type of connections
      using ConnectionID =
          std::unique_ptr<::elastica::interface::connections::ConnectionBase<
              ConnectionsRegistrars>>;
      //************************************************************************

      //**Friendships***********************************************************
      //! Friend the Algorithm
      friend Algorithm;
      //! Friendship with Serialize
      friend struct ::elastica::io::Serialize<This>;
      //************************************************************************

     protected:
      //**Constructors**********************************************************
      /*!\name Constructors */
      //@{
      //************************************************************************
      /*!\brief Constructor of the Connecters class.
       */
      Connections() : n_total_connections(0UL), connections_(), connecters_() {
        using namespace ::elastica::logging;
        log<info>([](auto& log) {
          log << "Successfully initialized the Connections module";
        });
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Move constructor of the Connecters class.
       *
       * \details This is not needed strictly (as it is ensured to be a
       * singleton class by the Simulator). However, while testing it is useful.
       */
      Connections(Connections&& other) noexcept
          : n_total_connections(other.n_total_connections),
            connections_(std::move(other.connections_)),
            connecters_(std::move(other.connecters_)){};
      //************************************************************************

      //@}
      //************************************************************************

     public:
      //**Destructor************************************************************
      /*!\name Destructor */
      //@{

      //************************************************************************
      /*!\brief Destructor
       */
      ~Connections() {
        using namespace ::elastica::logging;
        log<info>([](auto const& log) {
          log << "Successfully cleaned up the Connections module";
        });
      };
      //************************************************************************

      //@}
      //************************************************************************

     protected:
      //************************************************************************
      /*!\name Internal index transformation methods */
      //@{

      //************************************************************************
      /*!\brief Gets a symbolic index (size_t or from_end) contained in
       * symbolic containers (at_t)
       */
      template <typename T>
      static inline constexpr auto get_symbolic_index(T) = delete;
      static inline constexpr auto get_symbolic_index(
          std::size_t index) noexcept {
        return index;
      }
      static inline constexpr auto get_symbolic_index(
          ::elastica::from_end end_index) noexcept {
        return end_index;
      }
      static inline constexpr auto get_symbolic_index(
          ::elastica::interface::detail::At_t<1, 0>&& index) noexcept {
        return std::get<0UL>(index.ints());
      }
      static inline constexpr auto get_symbolic_index(
          ::elastica::interface::detail::At_t<0, 1>&& end_index) noexcept {
        return std::get<0UL>(end_index.ends());
      }
      //************************************************************************

      //@}
      //************************************************************************

      //**Interfaces************************************************************
      /*!\name Interface methods */
      //@{
     protected:
      //************************************************************************
      /*!\brief Check if the systems can be connected
       *
       * \param first_system The first system to be connected
       * \param second_system The second system to be connected
       * \exception std::nested_exception, if connection is not possible
       */
      template <typename FirstSystem, typename SecondSystem>
      inline void check_if_systems_can_be_connected(
          FirstSystem const& first_system,
          SecondSystem const& second_system) const {
        // TODO : Have names for systems to print in backtrace
        const char* error_message =
            "Attempts to connect systems failed because ";

        // 1. Check if not final
        auto finalize_check = [this, error_message]() {
          try {
            this->self().check_if_not_final();
          } catch (...) {
            std::throw_with_nested(std::runtime_error(error_message));
          }
        };

        // 2. Check if system is registered in the simulation
        auto registration_check = [this, error_message](auto const& system) {
          try {
            this->self().confirm_as_registered(system);
          } catch (...) {
            std::throw_with_nested(std::runtime_error(error_message));
          };
        };

        (finalize_check(),
         cpp17::apply(
             [registration_check =
                  std::move(registration_check)](auto const&... systems) {
               EXPAND_PACK_LEFT_TO_RIGHT(registration_check(systems));
             },
             // propagate references
             std::tie(first_system, second_system)));
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Connect two systems
       *
       * \details
       * connect() is the only interface for connecting systems within a
       * \elastica simulation.
       *
       * \param first_system The first system to be connected
       * \param second_system The second system to be connected
       * \param name An optional name for the connection, useful for debugging
       * \exception std::runtime_error If `first_system` and `second_system`
       * are not registered in the simulator
       * \return Mutable reference to a
       * ::elastica::interface::connections::Connection object
       *
       * \example
       * For the setup of systems shown below...
       * \snippet Connections/Test_Module.cpp connect_setup
       * ...and after launching a connect(), one can complete the connection
       * information in multiple ways, seen below for the same systems:
       * \snippet Connections/Test_Module.cpp connect_eg
       * \snippet Connections/Test_Module.cpp connect_eg_curry
       * \snippet Connections/Test_Module.cpp connect_eg_no_curry
       */
      template <typename FirstSystem, typename SecondSystem
                // Requires
                // <cpp17::conjunction_v<tt::is_block_slice<FirstSystem>,
                // tt::is_block_slice<SecondSystem>>> = nullptr
                >
      inline auto connect(FirstSystem const& first_system,
                          SecondSystem const& second_system,
                          std::string name = "")
          -> ::elastica::interface::connections::Connection<
              FirstSystem, SecondSystem, ConnectionsRegistrars>& {
        try {
          // 1. See if connections are possible between the systems
          check_if_systems_can_be_connected(first_system, second_system);

          // 2. if so, good create a connection object, append and return to
          // user
          using Connexion = ::elastica::interface::connections::Connection<
              FirstSystem, SecondSystem, ConnectionsRegistrars>;
          connections_.emplace_back(
              std::make_unique<Connexion>(n_total_connections, first_system,
                                          second_system, std::move(name)));

        } catch (const std::exception& e) {
          abort_with_backtrace(e);
        }

        // 3. increment counter
        ++n_total_connections;

        return static_cast<::elastica::interface::connections::Connection<
            FirstSystem, SecondSystem, ConnectionsRegistrars>&>(
            *(connections_.back()));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Begin a connection from one system
       *
       * \details
       * connect() is the only interface for connecting systems within a
       * \elastica simulation. This overload begins the connection from one
       * system and passes it on the next handler in the responsibility chain.
       *
       * \param first_system The first system to be connected
       *
       * \example
       * For the setup of systems shown below...
       * \snippet Connections/Test_Module.cpp connect_setup
       * ... and after launching a connect(), one can complete the connection
       * information in multiple ways, seen below for the same systems:
       * \snippet Connections/Test_Module.cpp connect_to_eg
       * \snippet Connections/Test_Module.cpp connect_to_eg_curry
       * \snippet Connections/Test_Module.cpp connect_to_eg_no_curry
       */
      template <typename FirstSystem>
      // Requires
      // <tt::is_block_slice_v<FirstSystem>> = nullptr>
      inline auto connect(FirstSystem const& first_system) noexcept {
        return ::elastica::interface::make_wrapped_to(
            [this, &first_system](auto const& second_system) -> decltype(auto) {
              return this->connect(first_system, second_system);
            });
      }
      //************************************************************************
     private:
      //************************************************************************
      /*!\brief Implementation details for a connection of a single system
       * with a given index
       *
       * \param first_system The first system to be connected
       * \param first_index The index of the first system being connected
       */
      template <typename FirstSystem, typename Index>
      // Requires
      // <tt::is_block_slice_v<FirstSystem>> = nullptr>
      inline auto single_connect_impl(FirstSystem const& first_system,
                                      Index first_index) noexcept {
        return ::elastica::interface::make_wrapped_to(
            [this, first_index, &first_system](
                auto const& second_system,
                auto&& second_index) mutable -> decltype(auto) {
              return this->connect(first_system, second_system)
                  .at(::elastica::interface::pair(
                      first_index,
                      This::get_symbolic_index(
                          std::forward<decltype(second_index)>(second_index))));
            });
      }
      //************************************************************************

     public:
      //************************************************************************
      /*!\brief Begin a connection from one system with a numeral index
       *
       * \details
       * connect() is the only interface for connecting systems within a
       * \elastica simulation. This overload begins the connection from one
       * system, records its index to be connected and passes it on the next
       * handler in the responsibility chain.
       *
       * \param first_system The first system to be connected
       * \param first_index The index of the first system being connected
       *
       * \example
       * For the setup of systems shown below...
       * \snippet Connections/Test_Module.cpp connect_setup
       * and after launching a connect(), one can complete the connection
       * information in multiple ways, seen below for the same systems:
       * \snippet Connections/Test_Module.cpp connect_idx_eg
       * \snippet Connections/Test_Module.cpp connect_idx_eg_curry
       */
      template <typename FirstSystem>
      inline auto connect(FirstSystem const& first_system,
                          std::size_t first_index) noexcept {
        return single_connect_impl(first_system, first_index);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Begin a connection from one system with an end index
       *
       * \details
       * connect() is the only interface for connecting systems within a
       * \elastica simulation. This overload begins the connection from one
       * system, records its index (from the end) to be connected and passes it
       * on the next handler in the responsibility chain.
       *
       * \param first_system The first system to be connected
       * \param end_index The index of the first system, from the end, being
       * connected
       *
       * \example
       * For the setup of systems shown below...
       * \snippet Connections/Test_Module.cpp connect_setup
       * and after launching a connect(), one can complete the connection
       * information in multiple ways, seen below for the same systems:
       * \snippet Connections/Test_Module.cpp connect_end_idx_eg
       * \snippet Connections/Test_Module.cpp connect_end_idx_eg_curry
       */
      template <typename FirstSystem>
      inline auto connect(FirstSystem const& first_system,
                          ::elastica::from_end end_index) noexcept {
        return single_connect_impl(first_system, end_index);
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Begin a connection from one system with an at() index
       *
       * \details
       * connect() is the only interface for connecting systems within a
       * \elastica simulation. This overload begins the connection from one
       * system, records its index to be connected using
       * ::elastica::interface::at() and passes it on the next handler in the
       * responsibility chain.
       *
       * \param first_system The first system to be connected
       * \param first_index The index of the first system being connected,
       * constructed using ::elastica::interface::at()
       *
       * \example
       * For the setup of systems shown below...
       * \snippet Connections/Test_Module.cpp connect_setup
       * and after launching a connect(), one can complete the connection
       * information in multiple ways, seen below for the same systems:
       * \snippet Connections/Test_Module.cpp connect_at_idx_eg
       * \snippet Connections/Test_Module.cpp connect_at_idx_eg_curry
       */
      template <typename FirstSystem>
      inline auto connect(
          FirstSystem const& first_system,
          ::elastica::interface::detail::At_t<1, 0>&& first_index) {
        return connect(first_system,
                       This::get_symbolic_index(std::move(first_index)));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Begin a connection from one system with an end index
       *
       * \details
       * connect() is the only interface for connecting systems within a
       * \elastica simulation. This overload begins the connection from one
       * system, records its index (from the end) to be connected using
       * ::elastica::interface::at()  and passes it on the next handler in the
       * responsibility chain.
       *
       * \param first_system The first system to be connected
       * \param end_index The index of the first system, from the end, being
       * connected, constructed using ::elastica::interface::at()
       *
       * \example
       * For the setup of systems shown below...
       * \snippet Connections/Test_Module.cpp connect_setup
       * and after launching a connect(), one can complete the connection
       * information in multiple ways, seen below for the same systems:
       * \snippet Connections/Test_Module.cpp connect_at_end_idx_eg
       * \snippet Connections/Test_Module.cpp connect_at_end_idx_eg_curry
       */
      template <typename FirstSystem>
      inline auto connect(
          FirstSystem const& first_system,
          ::elastica::interface::detail::At_t<0, 1>&& end_index) {
        return connect(first_system,
                       This::get_symbolic_index(std::move(end_index)));
      }
      //************************************************************************

      //************************************************************************
      /*!\brief Apply connections in between systems
       *
       * \param time_v The current time of simulation
       */
      inline auto apply_connections(TimeType time_v) const -> void {
        // this is internal so a member function suffices.
        auto const& algorithm =
            self().template get_algorithm<typename Algorithm::ConnectSystems>();
        algorithm(time_v);
      };
      //************************************************************************

      //************************************************************************
      /*!\brief Gets the number of connections registered in the present
       * simulator
       */
      inline auto n_connections() const noexcept -> SizeType {
        return n_total_connections;
      }
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Gets the connecter buffer
       */
      inline constexpr auto connecters() const& noexcept
          -> std::vector<ConnecterID> const& {
        return connecters_;
      }
      //************************************************************************

      //@}
      //************************************************************************

     protected:
      //************************************************************************
      /*!\brief Check status of connections
       *
       * \details
       * Check for any missing connections information, and report it to the
       * user in a sane fashion.
       *
       * \exception std::runtime_error If any connections information is
       * missing/faulty
       * \return void
       */
      auto check_and_report_errors() const -> void {
        /*
         * Here we first check if f_ is empty
         * This happens when users do something along the lines of
         *   \code
         *     e = rod()
         *     f = rod()
         *     sim.push_back(e)
         *     sim.push_back(f)
         *     sim.connect(e, f)
         *   \endcode
         * Notice that there is an attempt to connect the rods, yet actual
         * connection information is not received. The simulator thinks they
         * have connected, and tries to access the connection generator.
         * Unfortunately its a nullptr which upon dereferencing results in a
         * segfault
         */
        using UUID = ::elastica::interface::connections::ConnectionUUID;
        struct ConnectionInfo {
          UUID uuid;
          std::string connection_typename;
          std::string first_system_typename;
          std::string second_system_typename;

          ConnectionInfo(UUID id, std::string ct, std::string fst,
                         std::string sst)
              : uuid(std::move(id)),
                connection_typename(std::move(ct)),
                first_system_typename(std::move(fst)),
                second_system_typename(std::move(sst)) {}
        };
        using FaultyConnectionsInfo = std::vector<ConnectionInfo>;
        FaultyConnectionsInfo faulty_connections_info;

        // assume 10% is faulty
        const std::size_t n_faulty_guess(n_total_connections / 10);
        faulty_connections_info.reserve(n_faulty_guess);
        std::for_each(
            connections_.cbegin(), connections_.cend(),
            [&faulty_connections_info](ConnectionID const& connection) {
              if (connection->is_not_valid())
                faulty_connections_info.emplace_back(
                    connection->id(), connection->concrete_connection_type(),
                    connection->first_concrete_system_type(),
                    connection->second_concrete_system_type());
            });

        if (not faulty_connections_info.empty()) {
          std::ostringstream s_stream;

          s_stream << R"error(
Faults detected in the following connections!
They are listed by their ids and names below:

)error";

          for (auto const& connection_info : faulty_connections_info) {
            auto const& connection_uuid = connection_info.uuid;
            auto const& concrete_typename = connection_info.connection_typename;
            auto const& first_system_typename =
                connection_info.first_system_typename;
            auto const& second_system_typename =
                connection_info.second_system_typename;
            s_stream << "ID : " << connection_uuid.uuid;
            if (not connection_uuid.name.empty())
              s_stream << "\t Connection Name : " << connection_uuid.name;
            if (not concrete_typename.empty())
              s_stream << "\t Connection Type : " << concrete_typename;
            if (not first_system_typename.empty())
              s_stream << "\t First System Type : " << first_system_typename;
            if (not second_system_typename.empty())
              s_stream << "\t Second System Type : " << second_system_typename;
            // TODO Have an option to tag/name each rod so that its easier to
            // detect which rod has errors.
            s_stream << "\n";
          }

          s_stream << R"error(
Did you forget to call the `with()` method while attempting to connect?
Did you give the right indices using the at() method?
)error";
          throw std::runtime_error(s_stream.str());
        }
      }
      //************************************************************************

      //************************************************************************
      /*!\brief The finalize method.
       *
       * \details
       * The finalize method is the interface to finalize the Connections module
       * of elastica before launching into temporal integration. It checks for
       * any missing information, then assembles all Connection information
       * received thus far to apply connections performantly
       *
       * \param time_v The time to finalize, usually 0.0
       * \exception std::runtime_error If any Connection information is
       * missing/faulty
       */
      void finalize(TimeType time_v) {
        try {
          check_and_report_errors();

          connecters_.reserve(n_total_connections);
          std::transform(connections_.cbegin(), connections_.cend(),
                         std::back_inserter(connecters_),
                         [](const ConnectionID& connection) {
                           return connection->finalize();
                         });
        } catch (...) {
          const char* error_message =
              "Preparing the connections module failed because";
          std::throw_with_nested(std::runtime_error(error_message));
        }

        // connections are no longer needed, remove them
        connections_.clear();
        connections_.shrink_to_fit();

        // to be safe, connect initially
        apply_connections(time_v);
      }
      //************************************************************************

     private:
      //**Member variables******************************************************
      //! The total number of registered connections
      SizeType n_total_connections;
      //! The temporary connection information before finalizing
      std::vector<ConnectionID> connections_;
      //! The connection information applied on runtime
      std::vector<ConnecterID> connecters_;
      //************************************************************************
    };
    //**************************************************************************

  }  // namespace modules

}  // namespace elastica
