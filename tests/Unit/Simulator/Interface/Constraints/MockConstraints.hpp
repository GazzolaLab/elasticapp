//
// Created by Tejaswin Parthasarathy on 6/23/21.
//

#pragma once

#include <memory>

#include "Simulator/Configuration/ModuleConfiguration/Constraints.hpp"
#include "Simulator/Interface/Constraints/ConstrainerBase.hpp"
#include "Simulator/Interface/Constraints/Constraint.hpp"
#include "Simulator/Interface/Constraints/ConstraintUUID.hpp"
#include "Simulator/Interface/Constraints/ExecutionPolicy.hpp"
#include "Simulator/Interface/Constraints/Module.hpp"
#include "Time/SimulationTime.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/NonCopyable.hpp"
/// seraizlie
#include "Simulator/Interface/Constraints/Serialize.hpp"

namespace mock {

  //////////////////////////////////////////////////////////////////////////////
  // Test of applicator
  //////////////////////////////////////////////////////////////////////////////
  class Constrainer final
      : public ::elastica::interface::constraints::ConstrainerBase {
   public:
    using Parent = ConstrainerBase;
    using typename Parent::TimeType;

    explicit Constrainer(int arg)
        : Parent(
              ::elastica::interface::constraints::ConstraintUUID(0UL, "test")),
          i{arg} {};
    explicit Constrainer(
        ::elastica::interface::constraints::ConstraintUUID uuid)
        : Parent(std::move(uuid)), i{2} {};
    ~Constrainer() final = default;

   public:
    using Parent::id;
    void constrain_primitives(TimeType) const noexcept override { i += 2; }
    void constrain_rates(TimeType) const noexcept override { --i; }
    void* get_constraint_unsafe() noexcept override {
      // imma play with fire.
      return nullptr;
    }
    int value() const { return i; };

   private:
    mutable int i;
  };

  //////////////////////////////////////////////////////////////////////////////
  // Test of constratin interface
  //////////////////////////////////////////////////////////////////////////////
  template <typename System, typename ConcreteConstraint>
  struct TestConstraintImpl
      : public ::elastica::interface::constraints::Constraint<
            System, ConcreteConstraint> {
   public:
    using Parent =
        ::elastica::interface::constraints::Constraint<System,
                                                       ConcreteConstraint>;
    using typename Parent::IndexType;

    // functions
    using Parent::at;
    using Parent::concrete_constraint_type;
    using Parent::concrete_system_type;
    using Parent::is_not_valid;
    using Parent::record_name_of;
    using Parent::unsafe_with;
    using Parent::with;

    // protected
    using Parent::finalize;  // gives a constrainer, we shouldn't test it here
   public:
    // respects access modifiers
    /// using Parent::Parent;  // todo protected

    explicit TestConstraintImpl(IndexType id, System& s, std::string&& name)
        : Parent(id, s, std::move(name)){};

    // meta, all protected
    using Parent::conformant_constraint;
    using Parent::conformant_index_set;
    using Parent::conformant_interface;
    using Parent::non_conformant_constraint;
    using Parent::non_conformant_index_set;
    using Parent::non_conformant_interface;
  };

  template <typename System>
  using TestConstraint = TestConstraintImpl<System, tmpl::list<>>;

  //////////////////////////////////////////////////////////////////////////////
  // Test of execution policy
  //////////////////////////////////////////////////////////////////////////////
  template <class ConstraintsTraits, class ConstraintsModule>
  class ExecutionPolicy
      : public ::elastica::interface::constraints::ExecutionPolicy<
            ConstraintsTraits, ConstraintsModule> {
   private:
    using P =
        ::elastica::interface::constraints::ExecutionPolicy<ConstraintsTraits,
                                                            ConstraintsModule>;

   public:
    using P::ConstrainPrimitivesOp;
    using P::ConstrainRatesOp;
  };

  template <typename ConstraintsTraits>
  struct SimulatorWithConstraints
      : public ExecutionPolicy<ConstraintsTraits,
                               SimulatorWithConstraints<ConstraintsTraits>>,
        protected ::elastica::NonCopyable {
    using C = Constrainer;
    using Pointer = std::unique_ptr<C>;
    using TimeType = typename ConstraintsTraits::TimeType;
    SimulatorWithConstraints() : constrainer_batches_() {}
    ~SimulatorWithConstraints() = default;
    SimulatorWithConstraints(SimulatorWithConstraints&&) noexcept = default;

    using Policy = ExecutionPolicy<ConstraintsTraits,
                                   SimulatorWithConstraints<ConstraintsTraits>>;
    using PrimitiveAlgorithm = typename Policy::ConstrainPrimitives::type;
    using RateAlgorithm = typename Policy::ConstrainRates::type;

    // n_con
    std::size_t n_constraints() const {
      return std::accumulate(std::cbegin(constrainer_batches()),
                             std::cend(constrainer_batches()), 0UL,
                             [](std::size_t sum, auto const& batch) {
                               return sum + batch.size();
                             });
    }
    using Container = std::vector<Pointer>;
    using CC = std::vector<Container>;
    inline constexpr auto constrainer_batches() & -> CC& {
      return constrainer_batches_;
    }
    inline constexpr auto constrainer_batches() const& -> CC const& {
      return constrainer_batches_;
    }
    CC constrainer_batches_;
  };

  //////////////////////////////////////////////////////////////////////////////
  // Test module
  //////////////////////////////////////////////////////////////////////////////
  template <class SimTraits, class Sim>
  class ConstraintsModule
      : public ::elastica::modules::Constraints<SimTraits, Sim> {
   public:
    using P = ::elastica::modules::Constraints<SimTraits, Sim>;

    //    template <typename FirstSystem, typename SecondSystem>
    //    inline decltype(auto) connect(FirstSystem const& first_system,
    //                                  SecondSystem const& second_system,
    //                                  std::string name = "") {
    //      return P::connect(first_system, second_system, std::move(name));
    //    }

    using P::check_and_report_errors;
    using P::check_if_system_can_be_constrained;
    using P::constrain;
    using P::constrainer_batches;
    using P::finalize;
  };

  //////////////////////////////////////////////////////////////////////////////
  // Configurations for the above tests
  //////////////////////////////////////////////////////////////////////////////
  template <typename... Args>
  inline constexpr auto make_constraints_configuration(
      Args&&... args) noexcept {
    auto config = ::elastica::configuration::make_constraints_configuration(
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
    struct Serialize<::mock::ConstraintsModule<SimTraits, Sim>> {
      using type = ::mock::ConstraintsModule<SimTraits, Sim>;

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
