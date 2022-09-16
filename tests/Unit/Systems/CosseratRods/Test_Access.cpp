//
// Created by Tejaswin Parthasarathy on 5/19/21.
//

#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods/Access.hpp"
#include "Systems/CosseratRods/Block.hpp"
#include "Systems/CosseratRods/BlockInitializer.hpp"
#include "Systems/CosseratRods/Components.hpp"
#include "Systems/CosseratRods/CosseratRodTraits.hpp"
#include "Systems/CosseratRods/Mocks.hpp"
#include "Systems/CosseratRods/Tags.hpp"

namespace {
  using ::elastica::cosserat_rod::CosseratRodPlugin;
  using ::elastica::cosserat_rod::DefaultCosseratRodTraits;
  using ::elastica::cosserat_rod::component::detail::RodSpanwiseGeometry;

  template <typename Plugin>
  using GenerateBlock = blocks::Block<Plugin>;

  template <typename Plugin>
  using GenerateBlockSlice = blocks::BlockSlice<Plugin>;

  template <template <typename, typename> class... T>
  using GenerateTestPlugin =
      CosseratRodPlugin<mock::MinimalCosseratRodTraits, blocks::Block, T...>;

  template <template <typename, typename> class... T>
  using GenerateTestSlicePlugin =
      CosseratRodPlugin<mock::MinimalCosseratRodTraits, blocks::BlockSlice,
                        T...>;

  ELASTICA_TEST_CASE("Unit.Systems.CosseratRods.access.minimal_traits",
                     "[Systems][Unit]") {
    using namespace elastica;
    using TestPlugin = GenerateTestPlugin<RodSpanwiseGeometry>;
    using Rod = GenerateBlock<TestPlugin>;
    Rod rod{};
    Rod const& const_rod = static_cast<Rod const&>(rod);
    using Traits = mock::MinimalCosseratRodTraits;
    using DataVector = typename Traits::DataType::Vector::type;
    using DataVectorSlice = typename Traits::DataType::Vector::slice_type;
    using DataVectorConstSlice =
        typename Traits::DataType::Vector::const_slice_type;

    const std::size_t new_n_elements(10UL);

    auto initializer(cosserat_rod::initialize_cosserat_rod<TestPlugin>(
        blocks::initialize<tags::NElement>([=]() { return new_n_elements; }),
        blocks::initialize<tags::Position>(
            [](std::size_t i) { return 3.0 + static_cast<double>(i); }),
        blocks::initialize<tags::Director>(
            [](std::size_t i) { return 2.0 + static_cast<double>(i); })));

    auto sliced_rod = rod.emplace(std::move(initializer));
    auto const& const_sliced_rod = const_rod[0UL];

    // 1. check for whole rod
    {
      DataVector expected_position_variable(rod.total_n_nodes());
      std::generate(expected_position_variable.begin(),
                    expected_position_variable.end(), [i = -1]() mutable {
                      ++i;
                      return 3.0 + static_cast<double>(i);
                    });
      CHECK_ITERABLE_APPROX(position(rod), expected_position_variable);
      CHECK_ITERABLE_APPROX(position(const_rod), expected_position_variable);

      DataVector expected_director_variable(rod.total_n_elements());
      std::generate(expected_director_variable.begin(),
                    expected_director_variable.end(), [i = -1]() mutable {
                      ++i;
                      return 2.0 + static_cast<double>(i);
                    });

      CHECK_ITERABLE_APPROX(director(rod), expected_director_variable);
      CHECK_ITERABLE_APPROX(director(const_rod), expected_director_variable);

      // 2. check for slice rod
      DataVectorSlice expected_position_slice_variable(
          expected_position_variable.begin(), expected_position_variable.end());
      CHECK_ITERABLE_APPROX(position(sliced_rod),
                            expected_position_slice_variable);
      DataVectorConstSlice expected_position_const_slice_variable(
          expected_position_variable.begin(), expected_position_variable.end());
      CHECK_ITERABLE_APPROX(position(const_sliced_rod),
                            expected_position_const_slice_variable);

      DataVectorSlice expected_director_slice_variable(
          expected_director_variable.begin(), expected_director_variable.end());
      CHECK_ITERABLE_APPROX(director(sliced_rod),
                            expected_director_slice_variable);
      DataVectorConstSlice expected_director_const_slice_variable(
          expected_director_variable.begin(), expected_director_variable.end());
      CHECK_ITERABLE_APPROX(director(const_sliced_rod),
                            expected_director_const_slice_variable);
    }
  }

}  // namespace
