//
// Created by Tejaswin Parthasarathy on 4/22/22.
//

#include <type_traits>

#include "Systems/common/Components.hpp"

/// [is_component_example]
template <typename Traits, typename Block>
struct SomeComponent
    : public elastica::Component<SomeComponent<Traits, Block>> {};

template <typename Traits, typename Block>
struct NotAComponent  // as it doesnt derive from Component template
{};

struct DefinitelyNotAComponent {};

// assume to be Defined
struct TraitsLike;
struct BlockLike;

using Test1 = SomeComponent<TraitsLike, BlockLike>;

struct DerivedComponent : public Test1 {};

static_assert(elastica::tt::IsComponent<Test1>::value,
              "Component type traits failure!");
static_assert(not elastica::tt::IsComponent<DerivedComponent>::value,
              "Component type traits failure!");

using Test2 = NotAComponent<TraitsLike, BlockLike>;
static_assert(not elastica::tt::IsComponent<Test2>::value,
              "Component type traits failure!");

using Test3 = DefinitelyNotAComponent;
static_assert(not elastica::tt::IsComponent<Test3>::value,
              "Component type traits failure!");
/// [is_component_example]

/// [is_geometry_component_example]
static_assert(not elastica::tt::IsGeometryComponent<Test1>::value,
              "Component type traits failure!");
static_assert(not elastica::tt::IsGeometryComponent<Test2>::value,
              "Component type traits failure!");
static_assert(not elastica::tt::IsGeometryComponent<Test3>::value,
              "Component type traits failure!");

template <typename Traits, typename Block>
struct SomeGeometryComponent
    : public elastica::GeometryComponent<SomeGeometryComponent<Traits, Block>> {
};
using Test5 = SomeGeometryComponent<TraitsLike, BlockLike>;
struct DerivedGeometryComponent : public Test5 {};

static_assert(elastica::tt::IsComponent<Test5>::value,
              "Component type traits failure!");
static_assert(elastica::tt::IsGeometryComponent<Test5>::value,
              "Component type traits failure!");
static_assert(
    not elastica::tt::IsGeometryComponent<DerivedGeometryComponent>::value,
    "Component type traits failure!");
/// [is_geometry_component_example]

/// [geometry_component_trait_eg]
static_assert(
    std::is_same_v<typename elastica::tt::GeometryComponentTrait<Test1>::type,
                     NoSuchType>,
    "Component type traits failure!");
static_assert(
    std::is_same_v<typename elastica::tt::GeometryComponentTrait<Test5>::type,
                     Test5>,
    "Component type traits failure!");
static_assert(std::is_same_v<typename elastica::tt::GeometryComponentTrait<
                                   DerivedGeometryComponent>::type,
                               Test5>,
              "Component type traits failure!");
/// [geometry_component_trait_eg]

/// [is_kinematics_component_example]
static_assert(not elastica::tt::IsKinematicsComponent<Test1>::value,
              "Component type traits failure!");
static_assert(not elastica::tt::IsKinematicsComponent<Test2>::value,
              "Component type traits failure!");
static_assert(not elastica::tt::IsKinematicsComponent<Test3>::value,
              "Component type traits failure!");

template <typename Traits, typename Block>
struct SomeKinematicsComponent : public elastica::KinematicsComponent<
                                     SomeKinematicsComponent<Traits, Block>> {};
using Test6 = SomeKinematicsComponent<TraitsLike, BlockLike>;
struct DerivedKinematicsComponent : public Test6 {};

static_assert(elastica::tt::IsComponent<Test6>::value,
              "Component type traits failure!");
static_assert(elastica::tt::IsKinematicsComponent<Test6>::value,
              "Component type traits failure!");
static_assert(
    not elastica::tt::IsKinematicsComponent<DerivedKinematicsComponent>::value,
    "Component type traits failure!");
/// [is_kinematics_component_example]

/// [kinematics_component_trait_eg]
static_assert(std::is_same_v<
                  typename elastica::tt::KinematicsComponentTrait<Test1>::type,
                  NoSuchType>,
              "Component type traits failure!");
static_assert(
    std::is_same_v<
        typename elastica::tt::KinematicsComponentTrait<Test6>::type, Test6>,
    "Component type traits failure!");
static_assert(std::is_same_v<typename elastica::tt::KinematicsComponentTrait<
                                   DerivedKinematicsComponent>::type,
                               Test6>,
              "Component type traits failure!");
/// [kinematics_component_trait_eg]
