//
// Created by Tejaswin Parthasarathy on 5/12/21.
//

#include <type_traits>

#include "Systems/CosseratRods/Components/helpers/Component.hpp"
#include "Systems/CosseratRods/Components/helpers/TypeTraits.hpp"

/// [is_component_example]
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

template <typename C>
using IsComponent = elastica::cosserat_rod::component::IsComponent<C>;

using Test1 = SomeComponent<TraitsLike, BlockLike>;
static_assert(IsComponent<Test1>::value, "Component type traits failure!");

using Test2 = NotAComponent<TraitsLike, BlockLike>;
static_assert(!IsComponent<Test2>::value, "Component type traits failure!");

using Test3 = DefinitelyNotAComponent;
static_assert(!IsComponent<Test3>::value, "Component type traits failure!");
/// [is_component_example]
