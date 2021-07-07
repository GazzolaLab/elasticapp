//
// Created by Tejaswin Parthasarathy on 5/1/21.
//

#include <type_traits>

#include "Systems/CosseratRods/Components/helpers/AdapterHelpers.hpp"
#include "Systems/CosseratRods/Components/helpers/Component.hpp"

/// [adapter_helper_eg]
template <typename Traits, typename Block>
struct SomeComponent : public elastica::cosserat_rod::component::Component<
                           SomeComponent<Traits, Block>> {};

template <typename /* Traits */, typename /* Block */,
          typename ComponentToBeAdapted>
struct FirstAdapter;
template <typename /* Traits */, typename /* Block */,
          typename ComponentToBeAdapted>
struct SecondAdapter;
template <typename /* Traits */, typename /* Block */,
          typename ComponentToBeAdapted>
struct ThirdAdapter;
template <typename /* Traits */, typename /* Block */,
          typename ComponentToBeAdapted>
struct FourthAdapter;

template <typename Traits, typename Block>
using OneAdapterType = typename elastica::cosserat_rod::component::Adapt<
    SomeComponent<Traits, Block>>::template with<FirstAdapter>;

using TestOneAdapterType = OneAdapterType<int, int>;
using ExpectedOneAdapterType = FirstAdapter<int, int, SomeComponent<int, int>>;
static_assert(std::is_same<TestOneAdapterType, ExpectedOneAdapterType>::value,
              "Adapt failure!");

template <typename Traits, typename Block>
using TwoAdapterType = typename elastica::cosserat_rod::component::Adapt<
    SomeComponent<Traits, Block>>::template with<FirstAdapter, ThirdAdapter>;
using TestTwoAdapterType = TwoAdapterType<int, float>;
using ExpectedTwoAdapterType =
    FirstAdapter<int, float,
                 ThirdAdapter<int, float, SomeComponent<int, float>>>;

static_assert(std::is_same<TestTwoAdapterType, ExpectedTwoAdapterType>::value,
              "Adapt failure!");
/// [adapter_helper_eg]

template <typename Traits, typename Block>
using AdaptedType = typename elastica::cosserat_rod::component::Adapt<
    SomeComponent<Traits, Block>>::template with<FirstAdapter, FourthAdapter,
                                                 SecondAdapter, ThirdAdapter>;
using TestAdaptedType = AdaptedType<double, char>;
using ExpectedAdaptedType = FirstAdapter<
    double, char,
    FourthAdapter<double, char,
                  SecondAdapter<double, char,
                                ThirdAdapter<double, char,
                                             SomeComponent<double, char>>>>>;
static_assert(std::is_same<TestAdaptedType, ExpectedAdaptedType>::value,
              "Adapt failure!");
