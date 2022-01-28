//
// Created by Tejaswin Parthasarathy on 6/20/21.
//

#pragma once

#include <memory>

#include "Simulator/Configuration/ModuleConfiguration/Connections.hpp"
#include "Simulator/Interface/Connections/ConnecterBase.hpp"
#include "Simulator/Interface/Connections/Connection.hpp"
#include "Simulator/Interface/Connections/ConnectionUUID.hpp"
#include "Simulator/Interface/Connections/ExecutionPolicy.hpp"
#include "Simulator/Interface/Connections/Module.hpp"
#include "Time/SimulationTime.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/NonCopyable.hpp"
/// seraizlie
#include "Simulator/Interface/Connections/Serialize.hpp"

namespace mock {

  //////////////////////////////////////////////////////////////////////////////
  // Test of applicator
  //////////////////////////////////////////////////////////////////////////////
  class Connecter final
      : public ::elastica::interface::connections::ConnecterBase {
   public:
    using Parent = ConnecterBase;
    using typename Parent::TimeType;

    explicit Connecter(int arg)
        : Parent(
              ::elastica::interface::connections::ConnectionUUID(0UL, "test")),
          i{arg} {};
    explicit Connecter(::elastica::interface::connections::ConnectionUUID uuid)
        : Parent(std::move(uuid)), i{2} {};
    ~Connecter() final = default;

   public:
    using Parent::id;
    void apply_forces(TimeType) const noexcept override { i += 2; }
    void apply_torques(TimeType) const noexcept override { --i; }
    void* get_connection_unsafe() noexcept override {
      // imma play with fire.
      return nullptr;
    }
    int value() const { return i; };

   private:
    mutable int i;
  };

  // Extension of interface Connection
  //////////////////////////////////////////////////////////////////////////////
  // Test of connection interface
  //////////////////////////////////////////////////////////////////////////////
  template <typename FirstSystem, typename SecondSystem,
            typename ConcreteConnection>
  struct TestConnectionImpl
      : public ::elastica::interface::connections::Connection<
            FirstSystem, SecondSystem, ConcreteConnection> {
   public:
    using Parent = ::elastica::interface::connections::Connection<
        FirstSystem, SecondSystem, ConcreteConnection>;
    using typename Parent::IndexType;

    // functions
    using Parent::at;
    using Parent::concrete_connection_type;
    using Parent::first_concrete_system_type;
    using Parent::is_not_valid;
    using Parent::record_name_of;
    using Parent::second_concrete_system_type;
    using Parent::unsafe_with;
    using Parent::with;

    // protected
    using Parent::finalize;  // gives a constrainer, we shouldn't test it here
   public:
    // respects access modifiers
    /// using Parent::Parent;  // todo protected

    explicit TestConnectionImpl(IndexType id, FirstSystem const& s1,
                                SecondSystem const& s2, std::string&& name)
        : Parent(id, s1, s2, std::move(name)){};

    // meta, all protected
    using Parent::conformant_connection;
    using Parent::conformant_index_pair;
    using Parent::conformant_interface;
    using Parent::non_conformant_connection;
    using Parent::non_conformant_index_pair;
    using Parent::non_conformant_interface;
  };

  template <typename FirstSystem, typename SecondSystem>
  using TestConnection =
      TestConnectionImpl<FirstSystem, SecondSystem, tmpl::list<>>;

  //////////////////////////////////////////////////////////////////////////////
  // Test of execution policy
  //////////////////////////////////////////////////////////////////////////////
  template <class ConnectionsTraits, class ConnectionsModule>
  class ExecutionPolicy
      : public ::elastica::interface::connections::ExecutionPolicy<
            ConnectionsTraits, ConnectionsModule> {
   private:
    using P =
        ::elastica::interface::connections::ExecutionPolicy<ConnectionsTraits,
                                                            ConnectionsModule>;

   public:
    using P::ConnectSystemsOp;
  };

  template <typename ConnectionsTraits>
  struct SimulatorWithConnections
      : public ExecutionPolicy<ConnectionsTraits,
                               SimulatorWithConnections<ConnectionsTraits>>,
        protected ::elastica::NonCopyable {
    using C = Connecter;
    using Pointer = std::unique_ptr<C>;
    using TimeType = typename ConnectionsTraits::TimeType;
    SimulatorWithConnections() : connecters_() {}
    ~SimulatorWithConnections() = default;
    SimulatorWithConnections(SimulatorWithConnections&&) noexcept = default;

    using Policy = ExecutionPolicy<ConnectionsTraits,
                                   SimulatorWithConnections<ConnectionsTraits>>;
    using Algorithm = typename Policy::ConnectSystems::type;
    // n_con
    std::size_t n_connections() const { return connecters_.size(); }
    using Container = std::vector<Pointer>;
    inline constexpr auto connecters() & -> Container& { return connecters_; }
    inline constexpr auto connecters() const& -> Container const& {
      return connecters_;
    }
    Container connecters_;
  };

  //////////////////////////////////////////////////////////////////////////////
  // Test module
  //////////////////////////////////////////////////////////////////////////////
  template <class SimTraits, class Sim>
  class ConnectionsModule
      : public ::elastica::modules::Connections<SimTraits, Sim> {
   public:
    using P = ::elastica::modules::Connections<SimTraits, Sim>;

    //    template <typename FirstSystem, typename SecondSystem>
    //    inline decltype(auto) connect(FirstSystem const& first_system,
    //                                  SecondSystem const& second_system,
    //                                  std::string name = "") {
    //      return P::connect(first_system, second_system, std::move(name));
    //    }

    using P::check_and_report_errors;
    using P::check_if_systems_can_be_connected;
    using P::connect;
    using P::connecters;
    using P::finalize;
    using P::get_symbolic_index;
    using P::reset_module;
  };

  //////////////////////////////////////////////////////////////////////////////
  // Configurations for the above tests
  //////////////////////////////////////////////////////////////////////////////
  template <typename... Args>
  inline constexpr auto make_connections_configuration(
      Args&&... args) noexcept {
    auto config = ::elastica::configuration::make_connections_configuration(
        std::forward<Args>(args)...);
    struct S : public decltype(config) {
      using P = decltype(config);
      using TimeType IGNORE_UNUSED = ::elastica::Time;
      explicit S(P config) : P(std::move(config)){};
      ~S() = default;
    };
    // using Config = tmpl::inherit<tmpl::type_<S>,
    // tmpl::type_<decltype(config)>>;
    return S(config);
  }

}  // namespace mock

namespace elastica {

  namespace io {

    template <class SimTraits, class Sim>
    struct Serialize<::mock::ConnectionsModule<SimTraits, Sim>> {
      using type = ::mock::ConnectionsModule<SimTraits, Sim>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& module) {
        ar(static_cast<typename type::P&>(module));
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& module) {
        ar(static_cast<typename type::P const&>(module));
      }
    };

  }  // namespace io

}  // namespace elastica