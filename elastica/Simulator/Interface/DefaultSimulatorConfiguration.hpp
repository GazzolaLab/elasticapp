#pragma once

// NOT POSSIBLE : fwd declare instead of include
#include <memory>
#include <vector>

#include "elastica/Systems/RigidBodyBase.hpp"
#include "elastica/Systems/SystemBase.hpp"
#include "elastica/Utilities/TMPL.hpp"
// All environment tags
#include <utility>

#include "elastica/Simulator/Environments/Primitives/PlanePrimitive.hpp"
#include "elastica/Simulator/Environments/Tags.hpp"
#include "elastica/Utilities/TypeTraits.hpp"

namespace elastica {

  namespace interface {

    // DONE : Config class for simulator and all its Interface classes
    namespace detail {
      template <typename T>
      struct AddSystemInterfaceType {
        using type = std::shared_ptr<T>;
      };

      template <typename SysID>
      struct AddSystemStorageType {
        using type = std::vector<SysID>;
      };

      template <typename SystemStorage, typename = void>
      struct RemoveSystemStorageType;

      template <typename SystemStorage>
      struct RemoveSystemStorageType<
          SystemStorage, std::enable_if_t<std::is_same<
                             typename AddSystemStorageType<
                                 typename SystemStorage::value_type>::type,
                             SystemStorage>::value>> {
        using type = typename SystemStorage::value_type;
      };

      //      template <typename SystemStorage>
      //      struct RemoveSystemStorageType{
      //        using type = typename SystemStorage::value_type;
      //

    }  // namespace detail

    struct DefaultSystemsConfiguration {
      using AdmissibleSystemCollections =
          tmpl::list<::elastica::rigidbody::Sphere>;
    };
    // TODO Type trait/protocol for valud Systems Configuration

    // TODO Should be in a traits namespace somewhere
    template <typename C, typename = void>
    struct IsSystemsConfiguration : std::false_type {};

    template <typename C>
    struct IsSystemsConfiguration<
        C, cpp17::void_t<typename C::AdmissibleSystemCollections>>
        : std::true_type {};

    // TODO Should be in a traits namespace somewhere
    template <typename SystemCollection>
    struct GetPrimitiveType {
      using type = typename SystemCollection::PrimitiveType;
    };

    // TODO Should be in a traits namespace somewhere
    template <typename C, typename = void>
    struct IsEnvironmentConfiguration : std::false_type {};

    template <typename C>
    struct IsEnvironmentConfiguration<
        C, cpp17::void_t<typename C::AdmissibleEnvironmentTags,
                         typename C::AdmissibleImposedPrimitives>>
        : std::true_type {};

    struct DefaultEnvironmentConfiguration {
      // TODO : Ensure ordering with collision last
      using AdmissibleEnvironmentTags =
          tmpl::list<collision::tags::CollisionDetectionTag>;

      //      using AdmissibleImposedPrimitives =
      //      tmpl::list<::elastica::rigidbody::Sphere>;
      //      using AdmissibleEnvironmentTags = tmpl::list<>;
      using AdmissibleImposedPrimitives = tmpl::list<collision::PlanePrimitive>;
    };
    // TODO Type trait/protocol for valud Environment Configuration

    // TODO : Configuration classes are always empty : Trait checking
    template <class... Configs>
    struct SimulatorConfiguration : Configs... {
      // TODO Do not need, consider replacing
      using SizeType = std::size_t;  // size of data-structures
      using IndexType = SizeType;    // indexing rods
      using TimeType = double;

      //      template <typename T>
      //      using AddSystemInterfaceType = detail::AddSystemInterfaceType<T>;
      template <typename T>
      using SystemInterfaceType =
          typename detail::AddSystemInterfaceType<T>::type;
      template <typename T>
      using AddSystemStorageType =
          typename detail::AddSystemStorageType<T>::type;
      template <typename T>
      using RemoveSystemStorageType =
          typename detail::RemoveSystemStorageType<T>::type;

      // TODO : Determine based on the parallel strategy what
      //        the storage is going to be?
      // Alias
      template <typename T>
      using AddPrimitiveStorageType = AddSystemStorageType<T>;
      template <typename T>
      using RemovePrimitiveStorageType = RemoveSystemStorageType<T>;
      template <typename T>
      using AddImposedPrimitiveStorageType = AddSystemStorageType<T>;
      template <typename T>
      using RemoveImposedPrimitiveStorageType = RemoveSystemStorageType<T>;

      // TODO : Should be private
      using Configurations = tmpl::list<Configs...>;

      using simulator_configuration_found_result =
          tmpl::find<Configurations, IsSystemsConfiguration<tmpl::_1>>;
      static_assert(
          tmpl::size<simulator_configuration_found_result>::value,
          "A valid simulator configuration has not been found, "
          "aborting compilation. Please check the concepts/requirements of a "
          "System Configuration and ensure you provide one");

      //
      // Systems Configuration
      //
      using SystemsConfiguration =
          tmpl::front<simulator_configuration_found_result>;
      using AdmissibleSystemCollections =
          typename SystemsConfiguration::AdmissibleSystemCollections;

      // Get admissible primitive list downstream
      using AdmissiblePrimitives =
          tmpl::remove_duplicates<tmpl::transform<AdmissibleSystemCollections,
                                                  GetPrimitiveType<tmpl::_1>>>;
      using PrimitiveCollection =
          tmpl::transform<AdmissiblePrimitives,
                          tmpl::bind<AddPrimitiveStorageType, tmpl::_1>>;
      using PrimitiveStorage = tmpl::as_tuple<PrimitiveCollection>;

      //
      // Environment Configuration
      //
      // Can be optional, in which case we cannot do a front, but rather check
      // if result is found and act accordingly
      using environment_configuration_find_result =
          tmpl::find<Configurations, IsEnvironmentConfiguration<tmpl::_1>>;
      using environment_configuration_found =
          tmpl::bool_<tmpl::size<environment_configuration_find_result>::value>;

      // Instantiate if needed Use only if needed
      //      using EnvironmentConfiguration =
      //          tmpl::if_<environment_configuration_found,
      //                    tmpl::front<environment_configuration_find_result>,
      //                    tmpl::empty_sequence>;

      using AdmissibleImposedPrimitives =
          typename tmpl::if_<environment_configuration_found,
                    tmpl::remove_duplicates<typename tmpl::front<
                        environment_configuration_find_result>::
                                                AdmissibleImposedPrimitives>,
                    tmpl::empty_sequence>::type;
      using ImposedPrimitiveCollection =
          tmpl::transform<AdmissibleImposedPrimitives,
                          tmpl::bind<AddImposedPrimitiveStorageType, tmpl::_1>>;
      using ImposedPrimitiveStorage =
          tmpl::as_tuple<ImposedPrimitiveCollection>;
      //      using AdmissibleSystems = tmpl::list<SystemBase,
      //      ::elastica::rigidbody::Sphere>; using AdmissibleSystems =
      //      tmpl::list<::elastica::rigidbody::Sphere>; using
      //      AdmissibleEnvironments = tmpl::list<>; using AdmissibleSystems =
      //      tmpl::list<SystemBase, double>;
    };

    using DefaultSimulatorConfiguration =
        SimulatorConfiguration<DefaultSystemsConfiguration,
                               DefaultEnvironmentConfiguration>;

  }  // namespace interface

}  // namespace elastica