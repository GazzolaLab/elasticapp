#pragma once

// NOT POSSIBLE : fwd declare instead of include
#include <memory>
#include <utility>
#include <vector>

#include "elastica/Simulator/Interface/TypeTraits.hpp"
#include "elastica/Utilities/TMPL.hpp"
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

    // DONE : Configuration classes are always empty : Trait checking
    //  using static assers in the SimulatorConfiguration
    template <class... Configs>
    struct SimulatorConfiguration : Configs... {
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

      //      // TODO : Private
      //      template <typename T>
      //      struct StorageTag : public T {
      //        using T::BodyType;
      //        using T::PrimitiveType;
      //        using type = typename AddSystemStorageType<BodyType>;
      //      };

      //////////////////////////////////////////////////////////////////////////
      // Time Configuration
      //////////////////////////////////////////////////////////////////////////
      // Requirements : In the future, we can have a simple value class here
      // for better control
      using TimeType = double;

      //////////////////////////////////////////////////////////////////////////
      // Domain Configuration
      //////////////////////////////////////////////////////////////////////////

      // 1. Simulation always needs a computational domain
      using domain_configuration_found =
          tmpl::found<Configurations, tt::IsDomainConfiguration<tmpl::_1>>;
      static_assert(domain_configuration_found::value,
                    "A conforming domain configuration has not been found, "
                    "aborting compilation. Please check the protocols of a "
                    "Domain Configuration and ensure you provide one");

      //////////////////////////////////////////////////////////////////////////
      // Systems Configuration
      //////////////////////////////////////////////////////////////////////////

      // 1. Simulation always needs some systems to integrate
      using simulator_configuration_found_result =
          tmpl::find<Configurations, tt::IsSystemConfiguration<tmpl::_1>>;
      static_assert(
          tmpl::size<simulator_configuration_found_result>::value,
          "A valid simulator configuration has not been found, "
          "aborting compilation. Please check the concepts/requirements of a "
          "System Configuration and ensure you provide one");

      using SystemsConfiguration =
          tmpl::front<simulator_configuration_found_result>;
      using AdmissibleSystemCollectionTags =
          typename SystemsConfiguration::AdmissibleSystemCollectionTags;

      // 2. Check with feedback if its a valid simulator configuration
      using SystemsConfigurationValidityChecker =
          tt::IsValidSystemConfiguration<SystemsConfiguration>;

      // 3. Now we can retrieve collections and primitives, alias lookup
      // almost free!
      template <typename Tag>
      using GetSystemCollectionType =
          typename tt::GetSystemCollection<Tag>::type;

      using AdmissibleSystemCollections =
          tmpl::transform<AdmissibleSystemCollectionTags,
                          tmpl::bind<GetSystemCollectionType, tmpl::_1>>;

      // Helper metafunction
      // Given a tag, return its corresponding primitive
      template <typename Tag>
      using GetPrimitiveType = typename tt::GetPrimitive<Tag>::type;

      // Get admissible primitive list downstream
      // Its better to store as a tuple rather than a tagged tuple
      // because it might not be a one-to-one mapping (some systems
      // can have the same primitives)
      // Instead, one can ise the GetPrimitiveType metafunction above
      using AdmissiblePrimitives = tmpl::remove_duplicates<
          tmpl::transform<AdmissibleSystemCollectionTags,
                          tmpl::bind<GetPrimitiveType, tmpl::_1>>>;

      using PrimitiveCollection =
          tmpl::transform<AdmissiblePrimitives,
                          tmpl::bind<AddPrimitiveStorageType, tmpl::_1>>;
      using PrimitiveStorage = tmpl::as_tuple<PrimitiveCollection>;

      //////////////////////////////////////////////////////////////////////////
      // Environment Configuration
      //////////////////////////////////////////////////////////////////////////

      // 1. Can be optional, in which case we cannot do a front, but rather
      // check if result is found and act accordingly
      using environment_configuration_find_result =
          tmpl::find<Configurations, tt::IsEnvironmentConfiguration<tmpl::_1>>;
      using environment_configuration_found =
          tmpl::bool_<tmpl::size<environment_configuration_find_result>::value>;

      // TODO : Private
      //  Helper struct to cache the tmpl::if_ results and reduce unwanted
      //  if checks
      template <bool B, typename L>
      struct InternalConditional;

      template <typename FindResult>
      struct InternalConditional<true, FindResult> {
        using EnvironmentConfiguration = tmpl::front<FindResult>;
        using EnvironmentConfigurationValidityChecker =
            tt::IsValidEnvironmentConfiguration<EnvironmentConfiguration>;
        using AdmissibleImposedPrimitives = tmpl::remove_duplicates<
            typename EnvironmentConfiguration::AdmissibleImposedPrimitives>;
      };

      template <typename EmptyFindResult>
      struct InternalConditional<false, EmptyFindResult> {
        using EnvironmentConfiguration = tmpl::empty_sequence;
        using EnvironmentConfigurationValidityChecker = std::false_type;
        using AdmissibleImposedPrimitives = tmpl::empty_sequence;
      };

      // TODO : Private
      using EnvironmentFindResults =
          InternalConditional<environment_configuration_found::value,
                              environment_configuration_find_result>;

      // 2. Optional check
      using EnvironmentConfiguration =
          typename EnvironmentFindResults::EnvironmentConfiguration;

      // 3. If exists check if valid, with feedback to user
      using EnvironmentConfigurationValidityChecker =
          typename EnvironmentFindResults::
              EnvironmentConfigurationValidityChecker;

      /*
      using AdmissibleImposedPrimitives = typename tmpl::if_<
          environment_configuration_found,
          tmpl::remove_duplicates<
              typename tmpl::front<environment_configuration_find_result>::
                  AdmissibleImposedPrimitives>,
          tmpl::empty_sequence>::type;
      */

      // Even for imposed primitives, leave it as a tuple
      using AdmissibleImposedPrimitives =
          typename EnvironmentFindResults::AdmissibleImposedPrimitives;

      using ImposedPrimitiveCollection =
          tmpl::transform<AdmissibleImposedPrimitives,
                          tmpl::bind<AddImposedPrimitiveStorageType, tmpl::_1>>;

      using ImposedPrimitiveStorage =
          tmpl::as_tuple<ImposedPrimitiveCollection>;
    };

  }  // namespace interface

}  // namespace elastica