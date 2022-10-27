//
// Created by Tejaswin Parthasarathy on 2/17/22.
//

#pragma once

#include <utility>

#include "Simulator/Configuration/ModuleConfiguration/Environments.hpp"
#include "Simulator/Interface/Environments/ExecutionPolicy.hpp"
#include "Simulator/Interface/Environments/Module.hpp"
#include "Simulator/Utilities/Timers.hpp"
#include "Time/SimulationTime.hpp"
#include "Utilities/NonCopyable.hpp"
#include "Utilities/TMPL.hpp"

namespace mock {
  //////////////////////////////////////////////////////////////////////////////
  // Test of execution policy
  //////////////////////////////////////////////////////////////////////////////
  template <class EnvironmentsTraits, class EnvironmentsModule>
  class EnvExecutionPolicy
      : public ::elastica::interface::environments::ExecutionPolicy<
            EnvironmentsTraits, EnvironmentsModule> {
   private:
    using P = ::elastica::interface::environments::ExecutionPolicy<
        EnvironmentsTraits, EnvironmentsModule>;

   public:
    using P::ApplyEnvironmentForcesOp;
  };

  namespace detail {

    struct EnvironmentBase {
     protected:
      EnvironmentBase() = default;

      // Must come from the confgiuration files, leave for now
      using TimeType = ::elastica::Time;
      virtual void dispatch(TimeType) = 0;

     public:
      virtual ~EnvironmentBase() = default;

      template <typename Sim>
      void resolve(Sim&, TimeType t) {
        dispatch(t);
      }
    };

    // overloaded functions
    inline void reset(EnvironmentBase&, ::elastica::Time) noexcept {};
    template <typename CorrespondingEnvironmentConfiguration>
    inline void initialize_parameters(
        EnvironmentBase&,
        CorrespondingEnvironmentConfiguration const&) noexcept {};

    template <typename F>
    struct EnvironmentImpl final : public EnvironmentBase {
      explicit EnvironmentImpl(F a) : f(std::move(a)) {}
      void dispatch(TimeType t) override { f(t); }
      F f;
    };

    template <typename F>
    auto make_env_impl(F f) {
      return std::make_unique<EnvironmentImpl<F>>(std::move(f));
    }

  }  // namespace detail

  template <typename EnvironmentsTraits>
  struct SimulatorWithEnvironments
      : public EnvExecutionPolicy<
            EnvironmentsTraits, SimulatorWithEnvironments<EnvironmentsTraits>>,
        protected ::elastica::NonCopyable {
    using E = detail::EnvironmentBase;
    using Pointer = std::unique_ptr<E>;
    using TimeType = typename EnvironmentsTraits::TimeType;
    SimulatorWithEnvironments() = default;
    ~SimulatorWithEnvironments() = default;
    SimulatorWithEnvironments(SimulatorWithEnvironments&&) noexcept = default;

    using Policy =
        EnvExecutionPolicy<EnvironmentsTraits,
                           SimulatorWithEnvironments<EnvironmentsTraits>>;
    using Algorithm = typename Policy::ApplyEnvironmentForces::type;

    // n_con
    static constexpr std::size_t n_environments() { return 3UL; }

    template <typename... F>
    void register_envs(std::tuple<F...> funcs) {
      index_apply<n_environments()>(
          [&, fs = std::move(funcs)](auto... Is) mutable {
            EXPAND_PACK_LEFT_TO_RIGHT(
                std::get<Is>(environments()) =
                    detail::make_env_impl(std::get<Is>(std::move(fs))));
          });
    }
    // conformance with execution policy needing to reset forces
    template <typename F>
    void for_each_primitive(F) const noexcept {}
    // why?
    template <typename F>
    void for_each_imposed_primitive(F) const noexcept {}

    using Pointers = tmpl::filled_list<Pointer, n_environments()>;
    using Container = tmpl::as_tuple<Pointers>;
    inline constexpr auto environments() & -> Container& { return envs; }
    inline constexpr auto environments() const& -> Container const& {
      return envs;
    }

    using AdmissibleEnvironmentTags = tmpl::integral_list<std::size_t, 0, 1, 2>;
    using Timers = elastica::Timers<AdmissibleEnvironmentTags>;

    template <typename Tag>
    inline constexpr auto get_environment() & -> Pointer& {
      return std::get<Tag::value>(envs);
    }

    template <typename Tag>
    inline constexpr auto get_environment() const& -> Pointer const& {
      return std::get<Tag::value>(envs);
    }

    template <typename Tag>
    inline constexpr decltype(auto) get_environment_timer() & {
      using ::elastica::get_meter;
      return get_meter<Tag>(timers);
    }

    template <typename Tag>
    inline constexpr decltype(auto) get_environment_timer() const& {
      using ::elastica::get_meter;
      return get_meter<Tag>(timers);
    }

    Container envs = {nullptr, nullptr, nullptr};
    Timers timers;
  };

  struct EnvInternalTraits {
    using TimeType IGNORE_UNUSED = ::elastica::Time;
    using SizeType = std::size_t;
    using IndexType = std::size_t;
    using TimeDeltaType IGNORE_UNUSED = ::elastica::TimeDelta;
    template <typename T>
    using AddImposedPrimitiveStorageType = std::vector<T>;
  };

  //////////////////////////////////////////////////////////////////////////////
  // Configurations for the above tests
  //////////////////////////////////////////////////////////////////////////////
  template <typename ImposedPrimitives =
                ELASTICA_DEFAULT_ADMISSIBLE_IMPOSED_PRIMITIVES,
            typename... Args>
  inline constexpr auto make_environments_configuration(
      Args&&... args) noexcept {
    auto config = ::elastica::configuration::make_environments_configuration<
        ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS, ImposedPrimitives>(
        std::forward<Args>(args)...);
    struct S : public decltype(config), public EnvInternalTraits {
      using P = decltype(config);
      using I IGNORE_UNUSED = EnvInternalTraits;
      explicit S(P config) : P(std::move(config)), I(){};
      S(S const&) = default;
      S(S&&) noexcept = default;
      ~S() = default;
    };
    // using Config = tmpl::inherit<tmpl::type_<S>,
    // tmpl::type_<decltype(config)>>;
    return S(config);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Configurations for the above tests
  //////////////////////////////////////////////////////////////////////////////
  template <typename Tags = ELASTICA_DEFAULT_ADMISSIBLE_ENVIRONMENT_TAGS,
            typename... Args>
  inline constexpr auto make_environments_configuration_with_tags(
      Args&&... args) noexcept {
    auto config = ::elastica::configuration::make_environments_configuration<
        Tags, tmpl::list<>>(std::forward<Args>(args)...);
    struct S : public decltype(config), public EnvInternalTraits {
      using P = decltype(config);
      using I IGNORE_UNUSED = EnvInternalTraits;
      explicit S(P config) : P(std::move(config)), I(){};
      S(S const&) = default;
      S(S&&) noexcept = default;
      ~S() = default;
    };
    // using Config = tmpl::inherit<tmpl::type_<S>,
    // tmpl::type_<decltype(config)>>;
    return S(config);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Test module
  //////////////////////////////////////////////////////////////////////////////
  template <class SimTraits, class Sim>
  class EnvironmentsModule
      : public ::elastica::modules::Environments<SimTraits, Sim> {
   public:
    using P = ::elastica::modules::Environments<SimTraits, Sim>;

    //    template <typename FirstSystem, typename SecondSystem>
    //    inline decltype(auto) connect(FirstSystem const& first_system,
    //                                  SecondSystem const& second_system,
    //                                  std::string name = "") {
    //      return P::connect(first_system, second_system, std::move(name));
    //    }

    using P::environments;
    using P::finalize;
    using P::reset_module;
  };

}  // namespace mock

namespace elastica {

  namespace io {

    template <class SimTraits, class Sim>
    struct Serialize<::mock::EnvironmentsModule<SimTraits, Sim>> {
      using type = ::mock::EnvironmentsModule<SimTraits, Sim>;

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
