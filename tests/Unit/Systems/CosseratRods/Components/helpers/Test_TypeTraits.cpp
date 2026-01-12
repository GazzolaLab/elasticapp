//
// Created by Tejaswin Parthasarathy on 5/12/21.
//

#include <type_traits>

#include "Systems/CosseratRods/Components/helpers/Component.hpp"
#include "Systems/CosseratRods/Components/helpers/TypeTraits.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

template <typename Traits, typename Block>
struct SomeComponent : public elastica::cosserat_rod::component::Component<
                           SomeComponent<Traits, Block>> {};

template <typename Traits, typename Block>
struct NotAComponent  // as it doesnt derive from Component template
{};

struct DefinitelyNotAComponent {};

// assume to be Defined
struct TraitsLike;
struct BlockLike;

using Test1 = SomeComponent<TraitsLike, BlockLike>;
using Test2 = NotAComponent<TraitsLike, BlockLike>;
using Test3 = DefinitelyNotAComponent;

/// [is_elasticity_component_example]

template <typename T>
using IsComponent = ::elastica::cosserat_rod::component::tt::IsComponent<T>;
template <typename T>
using IsElasticityComponent =
    ::elastica::cosserat_rod::component::tt::IsElasticityComponent<T>;

static_assert(not IsElasticityComponent<Test1>::value,
              "Component type traits failure!");
static_assert(not IsElasticityComponent<Test2>::value,
              "Component type traits failure!");
static_assert(not IsElasticityComponent<Test3>::value,
              "Component type traits failure!");

template <typename Traits, typename Block>
struct SomeElasticityComponent
    : public ::elastica::cosserat_rod::component::ElasticityComponent<
          SomeElasticityComponent<Traits, Block>> {};
using Test5 = SomeElasticityComponent<TraitsLike, BlockLike>;
struct DerivedElasticityComponent : public Test5 {};

static_assert(IsComponent<Test5>::value, "Component type traits failure!");
static_assert(IsElasticityComponent<Test5>::value,
              "Component type traits failure!");
static_assert(not IsElasticityComponent<DerivedElasticityComponent>::value,
              "Component type traits failure!");
/// [is_elasticity_component_example]

/// [elasticity_component_trait_eg]

template <typename T>
using ElasticityComponentTrait =
    ::elastica::cosserat_rod::component::tt::ElasticityComponentTrait<T>;

static_assert(std::is_same_v<typename ElasticityComponentTrait<Test1>::type,
                               NoSuchType>,
              "Component type traits failure!");
static_assert(
    std::is_same_v<typename ElasticityComponentTrait<Test5>::type, Test5>,
    "Component type traits failure!");
static_assert(
    std::is_same_v<
        typename ElasticityComponentTrait<DerivedElasticityComponent>::type,
        Test5>,
    "Component type traits failure!");
/// [elasticity_component_trait_eg]
