//
// Created by Tejaswin Parthasarathy on 6/24/21.
//

#pragma once

#include <memory>

#include "Simulator/Configuration/ModuleConfiguration/Forcing.hpp"
#include "Simulator/Interface/Forcing/ExecutionPolicy.hpp"
#include "Simulator/Interface/Forcing/ForcerBase.hpp"
#include "Simulator/Interface/Forcing/Forcing.hpp"
#include "Simulator/Interface/Forcing/ForcingUUID.hpp"
#include "Simulator/Interface/Forcing/Module.hpp"
#include "Time/SimulationTime.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/NonCopyable.hpp"
/// seraizlie
#include "Simulator/Interface/Forcing/Serialize.hpp"

namespace mock {

  //////////////////////////////////////////////////////////////////////////////
  // Test of applicator
  //////////////////////////////////////////////////////////////////////////////
  class Forcer final : public ::elastica::interface::forcing::ForcerBase {
   public:
    using Parent = ForcerBase;
    using typename Parent::TimeType;

    explicit Forcer(int arg)
        : Parent(::elastica::interface::forcing::ForcingUUID(0UL, "test")),
          i{arg} {};
    explicit Forcer(::elastica::interface::forcing::ForcingUUID uuid)
        : Parent(std::move(uuid)), i{2} {};
    ~Forcer() final = default;

   public:
    using Parent::id;
    void apply_forces(TimeType) const noexcept override { i += 2; }
    void apply_torques(TimeType) const noexcept override { --i; }
    void* get_forcing_unsafe() noexcept final { return nullptr; }

    int value() const { return i; };

   private:
    mutable int i;
  };

  //////////////////////////////////////////////////////////////////////////////
  // Test of constratin interface
  //////////////////////////////////////////////////////////////////////////////
  template <typename System, typename ConcreteForcing>
  struct TestForcingImpl
      : public ::elastica::interface::forcing::Forcing<System,
                                                       ConcreteForcing> {
   public:
    using Parent =
        ::elastica::interface::forcing::Forcing<System, ConcreteForcing>;
    using typename Parent::IndexType;

    // functions
    using Parent::at;
    using Parent::concrete_forcing_type;
    using Parent::concrete_system_type;
    using Parent::is_not_valid;
    using Parent::record_name_of;
    using Parent::unsafe_with;
    using Parent::with;

    // protected
    using Parent::finalize;  // gives a forcer, we shouldn't test it here
   public:
    // respects access modifiers
    /// using Parent::Parent;  // todo protected

    explicit TestForcingImpl(IndexType id, System& s, std::string&& name)
        : Parent(id, s, std::move(name)){};

    // meta, all protected
    using Parent::conformant_forcing;
    using Parent::conformant_index_set;
    using Parent::conformant_interface;
    using Parent::non_conformant_forcing;
    using Parent::non_conformant_index_set;
    using Parent::non_conformant_interface;
  };

  template <typename System>
  using TestForcing = TestForcingImpl<System, tmpl::list<>>;

  //////////////////////////////////////////////////////////////////////////////
  // Test of execution policy
  //////////////////////////////////////////////////////////////////////////////
  template <class ForcingTraits, class ForcingModule>
  class ExecutionPolicy
      : public ::elastica::interface::forcing::ExecutionPolicy<ForcingTraits,
                                                               ForcingModule> {
   private:
    using P = ::elastica::interface::forcing::ExecutionPolicy<ForcingTraits,
                                                              ForcingModule>;

   public:
    using P::ForceSystemsOp;
  };

  template <typename ForcingTraits>
  struct SimulatorWithForcing
      : public ExecutionPolicy<ForcingTraits,
                               SimulatorWithForcing<ForcingTraits>>,
        protected ::elastica::NonCopyable {
    using C = Forcer;
    using Pointer = std::unique_ptr<C>;
    using TimeType = typename ForcingTraits::TimeType;
    SimulatorWithForcing() : forcer_batches_() {}
    ~SimulatorWithForcing() = default;
    SimulatorWithForcing(SimulatorWithForcing&&) noexcept = default;

    using Policy =
        ExecutionPolicy<ForcingTraits, SimulatorWithForcing<ForcingTraits>>;
    using Algorithm = typename Policy::ForceSystems::type;

    // n_con
    std::size_t n_forcings() const {
      return std::accumulate(std::cbegin(forcer_batches()),
                             std::cend(forcer_batches()), 0UL,
                             [](std::size_t sum, auto const& batch) {
                               return sum + batch.size();
                             });
    }
    using Container = std::vector<Pointer>;
    using CC = std::vector<Container>;
    inline constexpr auto forcer_batches() & -> CC& { return forcer_batches_; }
    inline constexpr auto forcer_batches() const& -> CC const& {
      return forcer_batches_;
    }
    CC forcer_batches_;
  };

  //////////////////////////////////////////////////////////////////////////////
  // Test module
  //////////////////////////////////////////////////////////////////////////////
  template <class SimTraits, class Sim>
  class ForcingModule : public ::elastica::modules::Forcing<SimTraits, Sim> {
   public:
    using P = ::elastica::modules::Forcing<SimTraits, Sim>;

    //    template <typename FirstSystem, typename SecondSystem>
    //    inline decltype(auto) connect(FirstSystem const& first_system,
    //                                  SecondSystem const& second_system,
    //                                  std::string name = "") {
    //      return P::connect(first_system, second_system, std::move(name));
    //    }

    using P::check_and_report_errors;
    using P::check_if_system_can_be_forced;
    using P::finalize;
    using P::force;
    using P::forcer_batches;
    using P::reset_module;
  };

  //////////////////////////////////////////////////////////////////////////////
  // Configurations for the above tests
  //////////////////////////////////////////////////////////////////////////////
  template <typename... Args>
  inline constexpr auto make_forcing_configuration(Args&&... args) noexcept {
    auto config = ::elastica::configuration::make_forcing_configuration(
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
    struct Serialize<::mock::ForcingModule<SimTraits, Sim>> {
      using type = ::mock::ForcingModule<SimTraits, Sim>;

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
