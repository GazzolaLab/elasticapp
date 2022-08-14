//
// Created by Tejaswin Parthasarathy on 7/23/22.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/IO/H5/CreateH5.hpp"
#include "Simulator/IO/H5/Systems/CosseratRods.hpp"
//
#include "Systems/CosseratRods.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Simulator.IO.H5.CosseratRodSlice",
                     "[Unit][Simulator][IO]") {
    auto group = generate_h5("CosseratRodSlice");
    // Setup
    using CosseratRod = ::elastica::cosserat_rod::CosseratRod;
    using Block = ::elastica::cosserat_rod::detail::CosseratRodBlock;
    Block block{};

    using StraightInit =
        ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    StraightInit straight_initializer{
        // StraightInitialization Wrapper
        StraightInit::NElement{10},
        StraightInit::Material{::elastica::iron},
        StraightInit::Radius{0.05},
        StraightInit::Length{10.0},
        StraightInit::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
        StraightInit::Direction{::elastica::Vec3{1.0, 0.0, 0.0}},
        StraightInit::Normal{::elastica::Vec3{0.0, 1.0, 0.0}},
        StraightInit::ForceDampingRate{0.01},
        StraightInit::TorqueDampingRate{0.01}};

    auto slice = block.emplace(straight_initializer.initialize<CosseratRod>());

    // Write and flush
    elastica::io::write_data(group, slice, "data");
    group.getFile().flush();

    // Clobber data
    Block another_block{};
    StraightInit another_straight_initializer{
        // StraightInitialization Wrapper
        StraightInit::NElement{10},
        StraightInit::Material{::elastica::copper},
        StraightInit::Radius{0.01},
        StraightInit::Length{2.0},
        StraightInit::Origin{::elastica::Vec3{1.0, 1.0, 1.0}},
        StraightInit::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
        StraightInit::Normal{::elastica::Vec3{0.0, 1.0, 0.0}},
        StraightInit::ForceDampingRate{0.02},
        StraightInit::TorqueDampingRate{0.02}};

    auto another_slice = another_block.emplace(
        another_straight_initializer.initialize<CosseratRod>());

    // Read data back
    elastica::io::read_data(group, another_slice, "data");

    // Post check
    CHECK(slice.data() == another_slice.data());
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.H5.CosseratRodWithoutDampingSlice",
                     "[Unit][Simulator][IO]") {
    auto group = generate_h5();
    // Setup
    using CosseratRod = ::elastica::cosserat_rod::CosseratRodWithoutDamping;
    using Block =
        ::elastica::cosserat_rod::detail::CosseratRodWithoutDampingBlock;
    Block block{};

    using StraightInit =
        ::elastica::cosserat_rod::StraightCosseratRodWithoutDampingInitializer;
    StraightInit straight_initializer{
        // StraightInitialization Wrapper
        StraightInit::NElement{10},
        StraightInit::Material{::elastica::iron},
        StraightInit::Radius{0.05},
        StraightInit::Length{10.0},
        StraightInit::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
        StraightInit::Direction{::elastica::Vec3{1.0, 0.0, 0.0}},
        StraightInit::Normal{::elastica::Vec3{0.0, 1.0, 0.0}}};

    auto slice = block.emplace(straight_initializer.initialize<CosseratRod>());

    // Write and flush
    elastica::io::write_data(group, slice, "data");
    group.getFile().flush();

    // Clobber data
    Block another_block{};
    StraightInit another_straight_initializer{
        // StraightInitialization Wrapper
        StraightInit::NElement{10},
        StraightInit::Material{::elastica::copper},
        StraightInit::Radius{0.01},
        StraightInit::Length{2.0},
        StraightInit::Origin{::elastica::Vec3{1.0, 1.0, 1.0}},
        StraightInit::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
        StraightInit::Normal{::elastica::Vec3{0.0, 1.0, 0.0}}};

    auto another_slice = another_block.emplace(
        another_straight_initializer.initialize<CosseratRod>());

    // Read data back
    elastica::io::read_data(group, another_slice, "data");

    // Post check
    CHECK(slice.data() == another_slice.data());
  }

}  // namespace
