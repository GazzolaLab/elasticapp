//
// Created by Tejaswin Parthasarathy on 10/3/22.
//
#include "Framework/TestingFramework.hpp"
#include "Systems/CosseratRods.hpp"
//
#include "Simulator/Forcing/InternalTorquesAdapter.hpp"
//
#include "Time/SimulationTime.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {
  using namespace elastica;

  template <typename CRT, template <typename> class ComputationalBlock,
            template <typename /*CRT*/, typename /*InitializedBlock*/>
            class... Components>
  Vec3 check_invariance(cosserat_rod::CosseratRodPlugin<
                            CRT, ComputationalBlock, Components...>& block_like,
                        std::size_t start, std::size_t stop) {
    Vec3 accum = {0.0, 0.0, 0.0};

    for (auto idx = start; idx < stop; ++idx) {
      accum += blaze::trans(::elastica::director(block_like, idx)) *
               ::elastica::external_torques(block_like, idx);
    }
    return accum;
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Forcing.InternalTorquesAdapter",
  "[Unit][Simulator][Forcing]") {
    // clang-format on
    // These always create a copy of the functors in question
    using StraightInit =
        ::elastica::cosserat_rod::StraightCosseratRodInitializer;
    StraightInit straight_initializer{
        // StraightInitialization Wrapper
        StraightInit::NElement{10UL},
        StraightInit::Material{::elastica::iron},
        StraightInit::Radius{0.02},
        StraightInit::Length{1.0},
        StraightInit::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
        StraightInit::Direction{::elastica::Vec3{1.0, 0.0, 0.0}},
        StraightInit::Normal{::elastica::Vec3{0.0, 1.0, 0.0}},
        StraightInit::ForceDampingRate{0.02},
        StraightInit::TorqueDampingRate{0.02},
    };
    using Block = ::elastica::cosserat_rod::detail::CosseratRodBlock;
    Block block{};
    auto slice = block.emplace(
        straight_initializer.initialize<cosserat_rod::CosseratRod>());

    auto dummy_func = [](real_t s, real_t) {
      // make sure this is varying
      return Vec3{s, 2 * s * s, 1.0};
    };
    ::elastica::Time time_v{0.0};

    elastica::forcing::apply_as_internal_torques(dummy_func, time_v, slice, 0UL,
                                                 10UL);
    CHECK(::elastica::external_torques(slice) != approx(0.0));

    auto v = check_invariance(slice, 0UL, 10UL);
    CAPTURE(v);
    CHECK(v[0] == approx(0.0));
    CHECK(v[1] == approx(0.0));
    CHECK(v[2] == approx(0.0));
  }

}  // namespace