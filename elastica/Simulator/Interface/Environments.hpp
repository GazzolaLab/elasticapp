#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "elastica/Simulator/Environments/Environments.hpp"
// TODO : Refactor this guy to simple environment within environment
#include "elastica/Simulator/Environments.hpp"
#include "elastica/Simulator/Environments/TypeTraits.hpp"
#include "elastica/Utilities/Logging.hpp"
#include "elastica/Utilities/NonCopyable.hpp"
#include "elastica/Utilities/Singleton.hpp"
#include "elastica/Utilities/StdHelpers.hpp"
#include "elastica/Utilities/TMPL.hpp"

namespace elastica {

  namespace environments {
    struct EnvironmentBase;
  }

  namespace interface {

    // TODO : tmpl::list of allowable environment types (basic, all, temporally
    // evolving)
    // TODO : protocol/requirement check for default constructible from
    // environments side
    template <class Sim,  // Simulator class, derived from this class using CRTP
              class SimulatorConfig>  // Simulator options
    struct Environments : public NonCopyable {
      using SizeType = typename SimulatorConfig::SizeType;
      using IndexType = typename SimulatorConfig::IndexType;
      using TimeType = typename SimulatorConfig::TimeType;
      /*
      template <typename T>
      using SystemInterfaceType =
          typename SimulatorConfig::template SystemInterfaceType<T>;
      */
      using EnvironmentID = std::unique_ptr<environments::EnvironmentBase>;
      //      using EnvironmentID = environments::EnvironmentBase*;

      using AdmissibleEnvironmentTags =
          typename SimulatorConfig::AdmissibleEnvironmentTags;
      using AdmissibleEnvironments =
          tmpl::transform<AdmissibleEnvironmentTags,
                          environments::tt::GetEnvironmentType<
                              tmpl::pin<SimulatorConfig>, tmpl::_1>>;
      // Singletons, tuple of shared_ptrs of environments
      using AdmissibleEnvironmentsStorage =
          tmpl::as_tuple<tmpl::transform<AdmissibleEnvironments,
                                         tt::AddSingletonStorage<tmpl::_1>>>;
      //      using AdmissibleEnvironments = tmpl::as_tuple<tmpl::transform<
      //          AdmissibleEnvironmentTags,
      //          environments::tags::GetEnvironmentType<collision::CollisionConfig
      //          , tmpl::_1>>>;

      template <typename ImposedPrimitive>
      using AddImposedPrimitiveStorageType =
          typename SimulatorConfig::template AddImposedPrimitiveStorageType<
              ImposedPrimitive>;

      using ImposedPrimitiveStorage =
          typename SimulatorConfig::ImposedPrimitiveStorage;

      //=================================================================================================
      //
      //  CONSTRUCTOR
      //
      //=================================================================================================

      //*************************************************************************************************
      /*!\brief Constructor of the Constraints class.
       */
      // TODO : mark protected
      // TODO : environments designed as singleton for now, revisit later
      Environments() : NonCopyable(), simple_envs_(), imposed_primitives_() {
        index_apply<get_number_of_environments()>([this](auto... Is) {
          EXPAND_PACK_LEFT_TO_RIGHT(
              std::get<Is>(environments_) =
                  tmpl::at_c<AdmissibleEnvironments, Is>::instance());
        });
        // We do not need to worry about the lifetime of the
        // logging class, the main simulator instance takes care of it
        LOG_INFO_SECTION(log) {
          log << "Successfully initialized the environment instance";
        }
      }

      ~Environments() {
        LOG_INFO_SECTION(log) {
          log << "Successfully destroyed the environment instance";
        }
      }

      template <typename EnvironmentType, typename... Args>
      auto add_simple_environment(Args&&... args) -> EnvironmentType* {
        static_assert(std::is_base_of<environments::EnvironmentBase,
                                      EnvironmentType>::value,
                      "Given environment does not satisfy interface "
                      "requirements, "
                      "did you forget to derive from EnvironmentBase?");
        // emplace back leaks https://stackoverflow.com/a/15784813
        simple_envs_.push_back(
            std::make_unique<EnvironmentType>(std::forward<Args>(args)...));
        return static_cast<EnvironmentType*>(simple_envs_.back().get());
        //        simple_envs_.push_back(
        //            static_cast<environments::EnvironmentBase*>(env.get()));
      }

      template <typename EnvironmentType>
      auto add_simple_environment(std::unique_ptr<EnvironmentType> env)
          -> EnvironmentType* {
        static_assert(std::is_base_of<environments::EnvironmentBase,
                                      EnvironmentType>::value,
                      "Given environment does not satisfy interface "
                      "requirements, "
                      "did you forget to derive from EnvironmentBase?");
        simple_envs_.push_back(std::move(env));
        return static_cast<EnvironmentType*>(simple_envs_.back().get());
      }

      //      template <typename EnvironmentType>
      //      void add_simple_environment(const
      //      std::shared_ptr<EnvironmentType>& env) {
      //        static_assert(std::is_base_of<environments::EnvironmentBase,
      //                                      EnvironmentType>::value,
      //                      "Given environment does not satisfy interface "
      //                      "requirements, "
      //                      "did you forget to derive from EnvironmentBase?");
      //        simple_envs_.push_back(
      //            static_cast<environments::EnvironmentBase*>(env.get()));
      //      }
      //
      constexpr static SizeType get_number_of_environments() noexcept {
        return tmpl::size<AdmissibleEnvironments>::value;
      }

     private:
      template <typename ImposedPrimitiveType>
      void apply_simple_environment_forces(
          std::vector<ImposedPrimitiveType>& system_collection,
          TimeType time_v) {
        for (auto& simple_env : simple_envs_) {
          for (ImposedPrimitiveType& sys_ptr : system_collection)
            simple_env->template apply<ImposedPrimitiveType>(sys_ptr, time_v);
        }
      }

      constexpr Sim const& self() const noexcept {
        return static_cast<const Sim&>(*this);
      }

      //      template <typename ImposedPrimitiveType>
      //      void apply_environment_forces(TimeType time_v) {
      //        auto& collection_containers_tuple =
      //            static_cast<Sim*>(this)->get_system_collection_storage();
      //        simple_env->template apply<ImposedPrimitiveType>(sys_ptr,
      //        time_v);
      //        }
      //      }
      //
     public:
      void resolve_effects(TimeType time_v) {
        // Resolve simple enviroments first
        // Earlier I was doing self().get_number_of_system_collections(), both
        // marked constexpr. It works in gcc and not in clang, so changed to
        // this version (static) instead
        constexpr SizeType n_system_collections(
            Sim::get_number_of_system_collections());
        index_apply<n_system_collections>([this, time_v](auto... Is) {
          EXPAND_PACK_LEFT_TO_RIGHT(this->apply_simple_environment_forces(
              static_cast<Sim*>(this)
                  ->template get_system_collection_container<Is>(),
              time_v));
        });

        // More complex environments, expansion of system collection of
        // parameter packs done inside
        auto& collection_containers_tuple =
            static_cast<Sim*>(this)->get_system_collection_storage();
        index_apply<get_number_of_environments()>(
            [this, &collection_containers_tuple, time_v](auto... Is) {
              EXPAND_PACK_LEFT_TO_RIGHT(
                  std::get<Is>(environments_)
                      ->resolve(collection_containers_tuple, time_v));
            });
        //          using expander = int[];
        //          (void)expander{
        //              0, ((void)(this->apply_simple_environment_forces(
        //                      static_cast<Sim*>(this)
        //                          ->template
        //                          get_system_collection_container<Is>(),
        //                      time_v)),
        //                  0)...};
        //        });
      }

      // TODO Make private?
      void finalize(TimeType /*time_v*/) {
        // 1. Call initialize on the more complex environments, passing the
        // current system storage and fixed primitives
        index_apply<get_number_of_environments()>([this](auto... Is) {
          EXPAND_PACK_LEFT_TO_RIGHT(
              std::get<Is>(environments_)
                  ->initialize(
                      static_cast<Sim*>(this)->get_system_collection_storage(),
                      &imposed_primitives_));
        });
      }

      // TODO : Private (outside world has no business accessing the vector of
      //        collections,
      template <typename ImposedPrimitive>
      inline constexpr auto get_imposed_primitive_container() noexcept
          -> AddImposedPrimitiveStorageType<ImposedPrimitive>& {
        return std::get<AddImposedPrimitiveStorageType<ImposedPrimitive>>(
            imposed_primitives_);
      }

      template <typename ImposedPrimitive>
      inline constexpr auto get_imposed_primitive_container() const noexcept
          -> AddImposedPrimitiveStorageType<ImposedPrimitive> const& {
        return std::get<AddImposedPrimitiveStorageType<ImposedPrimitive>>(
            imposed_primitives_);
      }

      template <class ImposedPrimitive>
      inline auto push_back_imposed_impl(ImposedPrimitive&& system)
          -> ImposedPrimitive* {
        // Propogate the fixed primitive to all (?!) the environments
        // while getting a pointer
        auto& imposed_primitive_container =
            get_imposed_primitive_container<ImposedPrimitive>();
        imposed_primitive_container.push_back(
            std::forward<ImposedPrimitive>(system));
        // return non-owning handle to the System just pushed back
        return &imposed_primitive_container.back();
      }

      /* For fixed objects (finite/non-finite) inserted in the simulator
       * Only used for 1) environments and 2) Visualization*/
      template <class ImposedPrimitive>
      inline auto push_back_imposed(ImposedPrimitive&& imposed_primitive) {
        // TODO proper error message
        static_assert(tmpl::list_contains_v<
                          typename SimulatorConfig::AdmissibleImposedPrimitives,
                          ImposedPrimitive> != 0,
                      " Oh no , not admitted!");

        return push_back_imposed_impl(
            std::forward<ImposedPrimitive>(imposed_primitive));
      }

      template <class ImposedPrimitive, class... Args>
      inline auto emplace_back_imposed_impl(Args&&... args) -> ImposedPrimitive* {
        auto& imposed_primitive_container =
            get_imposed_primitive_container<ImposedPrimitive>();
        imposed_primitive_container.emplace_back(std::forward<Args>(args)...);
        // return non-owning handle to the System just pushed back
        return &imposed_primitive_container.back();
      }

      template <class ImposedPrimitive, class... Args>
      inline auto emplace_back_imposed(Args&&... args) {
        // TODO proper error message
        static_assert(tmpl::list_contains_v<
                          typename SimulatorConfig::AdmissibleImposedPrimitives,
                          ImposedPrimitive> != 0,
                      " Oh no , not admitted!");

        return emplace_back_imposed_impl<ImposedPrimitive>(
            std::forward<Args>(args)...);
      }

     public:
      // TODO : Double indirection for slicing, not nice
      std::vector<EnvironmentID> simple_envs_;
      ImposedPrimitiveStorage imposed_primitives_;
      AdmissibleEnvironmentsStorage environments_;
    };
  }  // namespace interface
}  // namespace elastica
