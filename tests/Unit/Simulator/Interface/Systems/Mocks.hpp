//
// Created by Tejaswin Parthasarathy on 6/25/21.
//

#pragma once

#include <tuple>
#include <vector>

#include "Simulator/Configuration/ModuleConfiguration/Systems.hpp"
#include "Simulator/Interface/Systems/ExecutionPolicy.hpp"
#include "Simulator/Interface/Systems/Module.hpp"
///
#include "Time/SimulationTime.hpp"
///
#include "Utilities/NonCopyable.hpp"
#include "Utilities/TMPL.hpp"

namespace mock {

  //////////////////////////////////////////////////////////////////////////////
  // Test of execution policy
  //////////////////////////////////////////////////////////////////////////////
  template <class SystemsTraits, class SystemsModule>
  class ExecutionPolicy
      : public ::elastica::interface::systems::ExecutionPolicy<SystemsTraits,
                                                               SystemsModule> {
   private:
    using P = ::elastica::interface::systems::ExecutionPolicy<SystemsTraits,
                                                              SystemsModule>;

   public:
    // using P::ForEachSystemOp;
    using P::InvokeOverSystemsOp;
  };

  struct SystemA {
    std::size_t data;
  };
  struct SystemB {
    float data;
  };
  struct SystemC {
    char data;
  };

  template <typename SystemsTraits>
  struct SimulatorWithSystems
      : public ExecutionPolicy<SystemsTraits,
                               SimulatorWithSystems<SystemsTraits>>,
        protected ::elastica::NonCopyable {
    using TimeType = typename SystemsTraits::TimeType;
    SimulatorWithSystems(SystemsTraits system_traits)
        : system_collection_containers_(
              std::vector<SystemA>{SystemA{0UL}, SystemA{1UL}, SystemA{2UL},
                                   SystemA{3UL}},
              std::vector<SystemB>{SystemB{0.0F}, SystemB{1.0F}, SystemB{2.0F}},
              std::vector<SystemC>{SystemC{'0'}, SystemC{'1'}}),
          iteration_policy(std::move(system_traits).iteration_policy()) {}
    ~SimulatorWithSystems() = default;
    SimulatorWithSystems(SimulatorWithSystems&&) noexcept = default;

    using Policy =
        ExecutionPolicy<SystemsTraits, SimulatorWithSystems<SystemsTraits>>;
    using IterationPolicy = typename SystemsTraits::IterationPolicy;
    using Algorithm = typename Policy::ApplyOverSystems::type;

    //! Typelist of admissible systems
    //! \example <Sphere, Box, Cylinder, CosseratRod>
    using ListOfSystem = tmpl::list<SystemA, SystemB, SystemC>;
    using Systems = tmpl::list<std::vector<SystemA>, std::vector<SystemB>,
                               std::vector<SystemC>>;
    using SystemCollections = tmpl::as_tuple<Systems>;

    //************************************************************************
    /*!\brief Check if a collection should be iterated sequentially
     *
     * \param system_collection Collection of systems
     * \return bool true if collection should be iterated in a sequential
     * fashion, else else false
     */
    template <typename SystemCollection>
    inline auto to_be_iterated_sequentially(
        SystemCollection const& system_collection) const noexcept -> bool {
      return iteration_policy(system_collection);
    }
    //************************************************************************

    //************************************************************************
    /*!\brief Get method to return heterogeneous collection of systems
     *
     * \details
     * This method returns the entire storage, including all types registered
     * in the configuration. For internal use of \elastica developers, please
     * use with care.
     *
     * \return Mutable reference of the storage
     */
    inline constexpr auto get_system_collection_container() & noexcept
        -> SystemCollections& {
      return system_collection_containers_;
    }
    //************************************************************************

    //************************************************************************
    /*!\brief Get method to return heterogeneous collection of systems
     *
     * \details
     * This method returns the entire storage, including all types registered
     * in the configuration. For internal use of \elastica developers, please
     * use with care.
     *
     * \return Const reference of the storage
     */
    inline constexpr auto get_system_collection_container() const& noexcept
        -> SystemCollections const& {
      return system_collection_containers_;
    }
    //************************************************************************

    //************************************************************************
    /*!\brief Get method to return requested homogenous system collection
     *
     * \details
     * Get function for specific requested type of system container. For
     * internal use of \elastica developers, please use with care.
     *
     * \tparam SystemCollection Type of system collection requested
     * \return Mutable reference of the stored system collection
     */
    template <typename SystemCollection>
    inline constexpr auto get_system_collection_container() & noexcept
        -> std::vector<SystemCollection>& {
      return std::get<std::vector<SystemCollection>>(
          system_collection_containers_);
    }
    //************************************************************************

    //************************************************************************
    /*!\brief Get method to return requested homogenous system collection
     *
     * \details
     * Get function for specific requested type of system container. For
     * internal use of \elastica developers, please use with care.
     *
     * \tparam SystemCollection Type of system collection requested
     * \return Const reference of the stored system collection
     */
    template <typename SystemCollection>
    inline constexpr auto get_system_collection_container() const& noexcept
        -> std::vector<SystemCollection> const& {
      return std::get<std::vector<SystemCollection>>(
          system_collection_containers_);
    }
    //************************************************************************

    SystemCollections system_collection_containers_;
    IterationPolicy iteration_policy;
  };

  template <typename ListOfSystems>
  struct InternalTraits {
    using TimeType IGNORE_UNUSED = ::elastica::Time;
    using SizeType = std::size_t;
    using IndexType = std::size_t;
    using TimeDeltaType IGNORE_UNUSED = ::elastica::TimeDelta;
    template <typename T>
    using AddSystemStorageType = std::vector<T>;
    using ListOfAdmissibleSystemCollections = ListOfSystems;
  };

  template <typename ListOfSystems, typename... Args>
  inline constexpr auto make_systems_configuration(Args&&... args) noexcept {
    auto config = ::elastica::configuration::make_systems_configuration(
        std::forward<Args>(args)...);
    struct S : public decltype(config), public InternalTraits<ListOfSystems> {
      using P IGNORE_UNUSED = decltype(config);
      using I IGNORE_UNUSED = InternalTraits<ListOfSystems>;
      explicit S(P config) : P(std::move(config)), I(){};
      S(S const&) = default;
      S(S&&) noexcept = default;
      ~S() = default;
    };
    // using Config = tmpl::inherit<tmpl::type_<S>,
    // tmpl::type_<decltype(config)>>;
    return S(std::move(config));
  }

  template <typename... Args>
  inline constexpr auto make_systems_configuration(Args&&... args) noexcept {
    // systems configuration doesn't necessarily have a list of admissible
    // systems, it has some tag list
    auto config = ::elastica::configuration::make_systems_configuration(
        std::forward<Args>(args)...);
    struct S : public decltype(config) {
      using P = decltype(config);
      using TimeType IGNORE_UNUSED = ::elastica::Time;
      explicit S(P config) : P(std::move(config)){};
      S(S const&) = delete;
      S(S&&) noexcept = default;
      ~S() = default;
    };
    // using Config = tmpl::inherit<tmpl::type_<S>,
    // tmpl::type_<decltype(config)>>;
    return S(std::move(config));
  }

}  // namespace mock
