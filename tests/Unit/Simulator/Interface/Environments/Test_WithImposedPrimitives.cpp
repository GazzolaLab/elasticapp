//
// Created by Tejaswin Parthasarathy on 2/17/22.
//
#include <atomic>
#include <tuple>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Interface/Environments/MockEnvironments.hpp"
#include "Simulator/Interface/Environments/WithImposedPrimitives.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/Math/Identity.hpp"
#include "Utilities/Overloader.hpp"
#include "Utilities/ProtocolHelpers.hpp"
/// serializatoin
#include "Simulator/IO/Serialization.hpp"
#include "Simulator/Interface/Environments/Serialize.hpp"

//
#include "Simulator/Environments/Kinematics.hpp"
#include "Simulator/Environments/Primitives.hpp"
#include "Simulator/Frames.hpp"
#include "Simulator/Materials.hpp"
#include "Time/SimulationTime.hpp"
#include "Utilities/Invoke.hpp"

namespace mock {

  template <typename Traits>
  class WithImposedPrimitivesSimulator
      : public ::elastica::interface::environments::WithImposedPrimitives<
            Traits, WithImposedPrimitivesSimulator<Traits>> {
   public:
    using P = ::elastica::interface::environments::WithImposedPrimitives<
        Traits, WithImposedPrimitivesSimulator<Traits>>;
    using typename P::ImposedPrimitives;
    using typename P::ListOfAdmissibleImposedPrimitives;
    using TimeType = typename Traits::TimeType;

    ////////////  This piece of code is copy-pasted yo
    //! Get all algorithm steps
    using This = WithImposedPrimitivesSimulator<Traits>;
    using AlgorithmSteps = tmpl::append<typename P::PostKinematicUpdateSteps,
                                        typename P::PostDynamicUpdateSteps>;
    //! Algorithm cache type
    using Algorithms = tmpl::as_tagged_tuple<AlgorithmSteps>;
    static constexpr std::size_t n_algorithms =
        tmpl::size<AlgorithmSteps>::value;

    //! Cache for timers for profiling the code
    // using Timers = ::elastica::Timers<AlgorithmSteps>;
    ////////////  This piece of code is copy-pasted yo

    WithImposedPrimitivesSimulator(Traits traits, bool arg_is_final = false)
        : P(),
          ////////////  This piece of code is copy-pasted yo
          algorithms(cpp17::apply(
              [](auto&&... initializer_as_a_tuple) noexcept {
                return Algorithms(
                    std::forward<decltype(initializer_as_a_tuple)>(
                        initializer_as_a_tuple)...);
              },
              repeat_as_tuple<n_algorithms>(
                  // make_tuple doesn't work?
                  std::tuple<This*, Traits const&>(this, traits)))),
          ////////////  This piece of code is copy-pasted yo
          is_final(arg_is_final) {}
    ~WithImposedPrimitivesSimulator() = default;
    WithImposedPrimitivesSimulator(WithImposedPrimitivesSimulator&&) noexcept =
        default;

    // before
    using P::check_if_admissible;

    using P::imposed_primitives;

    using P::reset_module;

    // copied
    //************************************************************************
    /*!\brief Get an algorithm from the simulator cache
     *
     * \tparam AlgorithmTag Tag for obtaining the algorithm
     * \return Algorithm callable
     */
    template <typename AlgorithmTag>
    inline constexpr decltype(auto) get_algorithm() const noexcept {
      return tuples::get<AlgorithmTag>(algorithms);
    }
    //************************************************************************

    // ditto copy
    auto check_if_not_final() const -> void {
      const char* error_message = R"error(
The simulator has been finalized, yet you are trying to modify its states!
Please ensure that Simulator doesn't change state after finalize() is called by
reorganizing your code logic.
)error";
      if (ready())
        throw std::logic_error(error_message);
    }
    inline auto ready() const noexcept -> bool { return is_final; }
    void finalize(TimeType = {}) { is_final = true; }
    Algorithms algorithms;
    bool is_final;
  };

}  // namespace mock

namespace elastica {

  namespace io {

    template <class Traits>
    struct Serialize<::mock::WithImposedPrimitivesSimulator<Traits>> {
      using type = ::mock::WithImposedPrimitivesSimulator<Traits>;
      using actual_type = typename type::P;

      // 1. check if blocking is the same, else abort
      template <typename InArchive>
      static inline void load(InArchive& ar, type& module) {
        ar(static_cast<actual_type&>(module));
      }

      template <typename InArchive>
      static inline void save(InArchive& ar, type const& module) {
        ar(static_cast<actual_type const&>(module));
      }
    };

  }  // namespace io

}  // namespace elastica

namespace {

  template <typename ListOfImposedPrimitives>
  inline decltype(auto) make_imposed_systems_simulator() {
    auto make_config = []() {
      return ::mock::make_environments_configuration<ListOfImposedPrimitives>(
          elastica::configuration::seq);
    };
    auto config = make_config();
    //
    using Config = std::decay_t<decltype(config)>;
    using Sim = ::mock::WithImposedPrimitivesSimulator<Config>;
    return Sim(config);
  }

  using MockImposedPrimitiveOne = std::tuple<int, std::uint16_t>;
  using MockImposedPrimitiveTwo = std::tuple<char, double>;
  //  struct MockImposedPrimitiveOne {
  //    int i;
  //    float f;
  //  };
  //  struct MockImposedPrimitiveTwo {
  //    char c;
  //    double d;
  //  };

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Environment.WithImposedPrimitives.Module",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    EMPTY_TEST();

    using ListOfImposedPrimitives =
        tmpl::list<MockImposedPrimitiveOne, MockImposedPrimitiveTwo>;
    auto sim = make_imposed_systems_simulator<ListOfImposedPrimitives>();
    using Sim = decltype(sim);
    static_assert(Sim::get_number_of_imposed_primitive_collections() == 2,
                  "Systems meta-failure!");
    // can be another type
    // static_assert(
    //     cpp17::is_same_v<typename Sim::ImposedPrimitives,
    //                      std::tuple<std::vector<MockImposedPrimitiveOne>,
    //                                 std::vector<MockImposedPrimitiveTwo>>>,
    //     "Systems meta-failure!");
    Sim::check_if_admissible<MockImposedPrimitiveOne>();
    Sim::check_if_admissible<MockImposedPrimitiveTwo>();

    elastica::ignore_unused(sim);
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.WithImposedPrimitives.invalid_ref",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    EMPTY_TEST();

    using ListOfImposedPrimitives =
        tmpl::list<MockImposedPrimitiveOne, MockImposedPrimitiveTwo>;
    auto sim = make_imposed_systems_simulator<ListOfImposedPrimitives>();
    (void)(sim.get_invalid_ref<MockImposedPrimitiveOne>());
    (void)(sim.get_invalid_ref<MockImposedPrimitiveTwo>());
  }
  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Environment.WithImposedPrimitives.emplace_back",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    using ListOfImposedPrimitives =
        tmpl::list<MockImposedPrimitiveOne, MockImposedPrimitiveTwo>;
    auto sim = make_imposed_systems_simulator<ListOfImposedPrimitives>();

    using ::elastica::collision::get_collection;
    CHECK(get_collection<MockImposedPrimitiveOne>(sim.imposed_primitives())
              .empty());
    CHECK(get_collection<MockImposedPrimitiveTwo>(sim.imposed_primitives())
              .empty());

    auto imposed_primitive_ref = sim.get_invalid_ref<MockImposedPrimitiveOne>();
    auto another_imposed_primitive_ref =
        sim.get_invalid_ref<MockImposedPrimitiveTwo>();
    SECTION("Emplace back correctness") {
      imposed_primitive_ref =
          sim.emplace_back<MockImposedPrimitiveOne>(-1, 127U);

      CHECK(std::get<int>(imposed_primitive_ref.get()) == -1);
      CHECK(std::get<std::uint16_t>(imposed_primitive_ref.get()) == 127U);
      CHECK(std::get<int>(cpp17::as_const(imposed_primitive_ref).get()) == -1);
      CHECK(std::get<std::uint16_t>(
                cpp17::as_const(imposed_primitive_ref).get()) == 127U);

      another_imposed_primitive_ref =
          sim.emplace_back<MockImposedPrimitiveTwo>('a', 4.525);
      CHECK(std::get<char>(another_imposed_primitive_ref.get()) == 'a');
      CHECK(std::get<double>(another_imposed_primitive_ref.get()) ==
            approx(4.525));
      CHECK(std::get<char>(
                cpp17::as_const(another_imposed_primitive_ref).get()) == 'a');
      CHECK(std::get<double>(
                cpp17::as_const(another_imposed_primitive_ref).get()) ==
            approx(4.525));

      // check to see if setting is possible
      std::get<double>(another_imposed_primitive_ref.get()) = 2.0;
      CHECK(std::get<double>(another_imposed_primitive_ref.get()) ==
            approx(2.0));

      // doesn't work as expected
      // std::get<double>(cpp17::as_const(another_imposed_primitive_ref).get())
      // =
      //     4.525;
    }
    SECTION("Emplace back does not invalidate iterators") {
      // I think catch's section does something weird here.
      // If i don't have this, gsl::not_null complains here
      imposed_primitive_ref =
          sim.emplace_back<MockImposedPrimitiveOne>(-1, 127U);
      another_imposed_primitive_ref =
          sim.emplace_back<MockImposedPrimitiveTwo>('a', 4.525);

      for (std::size_t i = 0UL; i < 20UL; ++i) {
        sim.emplace_back<MockImposedPrimitiveOne>(-1 + int(i),
                                                  127U + std::uint8_t(i));
        sim.emplace_back<MockImposedPrimitiveTwo>(char(i),
                                                  4.525 + double(2 * i));
      }

      CHECK(std::get<int>(imposed_primitive_ref.get()) == -1);
      CHECK(std::get<std::uint16_t>(imposed_primitive_ref.get()) == 127U);

      CHECK(std::get<char>(another_imposed_primitive_ref.get()) == 'a');
      CHECK(std::get<double>(another_imposed_primitive_ref.get()) ==
            approx(4.525));
    }
    SECTION("Emplace back throws exception after finalize") {
      sim.finalize();
      CHECK_THROWS_WITH(sim.emplace_back<MockImposedPrimitiveTwo>('b', 234.2),
                        Catch::Contains("simulator has been finalized"));
    }
  }

  inline decltype(auto) setup_sim_with_state(const std::size_t n_units) {
    using ListOfImposedPrimitives =
        tmpl::list<MockImposedPrimitiveOne, MockImposedPrimitiveTwo>;
    auto sim = make_imposed_systems_simulator<ListOfImposedPrimitives>();
    auto initializers =
        std::make_tuple(std::make_tuple(-5, 45U), std::make_tuple('c', 450.03));
    // gives n_units / 2 blocks of ECBlock and n_units ECNamedBlock
    constexpr auto n_primitives = tmpl::size<ListOfImposedPrimitives>::value;
    auto add_primitives = [&](auto Is) {
      using P = tmpl::at_c<ListOfImposedPrimitives, Is>;
      auto initializer = std::get<Is>(initializers);
      for (auto i = 0UL; i < n_units; ++i)
        sim.emplace_back<P>(std::get<0UL>(initializer),
                            std::get<1UL>(initializer));
    };
    index_apply<n_primitives>(
        [&](auto... Is) { EXPAND_PACK_LEFT_TO_RIGHT(add_primitives(Is)); });
    return sim;
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Environment.WithImposedPrimitives.for_each_primitive",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    const std::size_t n_units = 4UL;
    auto sim = setup_sim_with_state(n_units);

    // apply at block granuliarity
    //    const std::size_t expected_n_ec_blocks = n_units / 2UL;
    //    const std::size_t expected_n_ec_named_blocks = n_units;

    using Counter = std::atomic<std::size_t>;
    std::array<Counter, sim.get_number_of_imposed_primitive_collections()>
        counters;
    std::array<Counter, sim.get_number_of_imposed_primitive_collections()>
        expected_counters;

    std::fill(std::begin(counters), std::end(counters), 0UL);
    std::fill(std::begin(expected_counters), std::end(expected_counters),
              n_units);

    auto func = index_apply<sim.get_number_of_imposed_primitive_collections()>(
        [&](auto... Is) {
          // using P = tmpl::at_c<typename
          // decltype(sim)::ListOfAdmissibleImposedPrimitives, Is>;
          return make_overloader(
              [&i = counters[Is]](
                  tmpl::at_c<
                      typename decltype(sim)::ListOfAdmissibleImposedPrimitives,
                      Is> const&) { ++i; }...);
        });

    sim.for_each_imposed_primitive(func);

    CHECK(counters == expected_counters);

    int sum = 0;
    sim.for_each_imposed_primitive<tmpl::list<MockImposedPrimitiveOne>>(
        [&sum, &i = counters[0]](auto const& sys) {
          ++i;
          sum += std::get<int>(sys);
        });
    expected_counters[0UL] += n_units;
    CHECK(counters == expected_counters);
    CHECK(sum == (-5 * int(n_units)));  // for my own sanity
  }

  inline decltype(auto) setup_sim_with_actual_imposed_systems() {
    using ListOfImposedPrimitives =
        tmpl::list<elastica::collision::ImposedSpherePrimitive,
                   elastica::collision::ImposedCapsulePrimitive>;
    auto sim = make_imposed_systems_simulator<ListOfImposedPrimitives>();
    auto initializers =
        std::make_tuple(std::make_tuple(-5, 45U), std::make_tuple('c', 450.03));
    // gives n_units / 2 blocks of ECBlock and n_units ECNamedBlock
    constexpr auto n_primitives = tmpl::size<ListOfImposedPrimitives>::value;

    elastica::Vec3 position{5.0, 3.0, 6.0};
    elastica::real_t radius(4.0);

    auto wrap = [](elastica::real_t inp) noexcept {
      return [=](elastica::real_t) noexcept -> elastica::real_t { return inp; };
    };

    auto pX = wrap(1.0);
    auto pY = wrap(2.0);
    auto pZ = wrap(3.0);

    auto vX = wrap(11.0);
    auto vY = wrap(12.0);
    auto vZ = wrap(13.0);

    auto translations_buffer = elastica::kinematics::make_translations(
        elastica::kinematics::translate_along<elastica::EulerianFrame::X>(
            elastica::kinematics::mark_position(pX),
            elastica::kinematics::mark_velocity(vX)),
        elastica::kinematics::translate_along<elastica::EulerianFrame::Y>(
            elastica::kinematics::mark_position(pY),
            elastica::kinematics::mark_velocity(vY)),
        elastica::kinematics::translate_along<elastica::EulerianFrame::Z>(
            elastica::kinematics::mark_position(pZ),
            elastica::kinematics::mark_velocity(vZ)));

    // kinematic constructor
    sim.emplace_back<elastica::collision::ImposedSpherePrimitive>(
        elastica::collision::SpherePrimitive(0UL, 0UL, true, position, radius,
                                             elastica::iron),
        translations_buffer);

    elastica::real_t length{12.0};
    sim.emplace_back<elastica::collision::ImposedCapsulePrimitive>(
        elastica::collision::CapsulePrimitive(1UL, 1UL, true, position,
                                              elastica::identity3(), radius,
                                              length, elastica::iron),
        translations_buffer);

    return sim;
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Environment.WithImposedPrimitives.update_values",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    auto sim = setup_sim_with_actual_imposed_systems();

    auto pre_update_primitive_check = [=](auto& p) {
      CHECK(p.get_position()[0] == approx(5.0));
      CHECK(p.get_position()[1] == approx(3.0));
      CHECK(p.get_position()[2] == approx(6.0));
    };

    auto post_update_primitive_check = [=](auto& p) {
      CHECK(p.get_position()[0] == approx(1.0));
      CHECK(p.get_position()[1] == approx(2.0));
      CHECK(p.get_position()[2] == approx(3.0));
    };

    sim.for_each_imposed_primitive(pre_update_primitive_check);

    ::elastica::Time time_v{0.0};
    sim.update_values_of_imposed_primitives(time_v);
    sim.for_each_imposed_primitive(post_update_primitive_check);
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Environment.WithImposedPrimitives.update_rates",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    auto sim = setup_sim_with_actual_imposed_systems();

    auto pre_update_primitive_check = [=](auto& p) {
      CHECK(p.get_velocity()[0] == approx(0.0));
      CHECK(p.get_velocity()[1] == approx(0.0));
      CHECK(p.get_velocity()[2] == approx(0.0));
    };

    auto post_update_primitive_check = [=](auto& p) {
      CHECK(p.get_velocity()[0] == approx(11.0));
      CHECK(p.get_velocity()[1] == approx(12.0));
      CHECK(p.get_velocity()[2] == approx(13.0));
    };

    sim.for_each_imposed_primitive(pre_update_primitive_check);

    ::elastica::Time time_v{0.0};
    sim.update_rates_of_imposed_primitives(time_v);
    sim.for_each_imposed_primitive(post_update_primitive_check);
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Environment.WithImposedPrimitives.n_imposed_primitives",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    const std::size_t n_units = 4UL;
    auto sim = setup_sim_with_state(n_units);

    CHECK(sim.n_imposed_primitives() ==
          (n_units * sim.get_number_of_imposed_primitive_collections()));
    CHECK(sim.n_imposed_primitives<MockImposedPrimitiveOne>() == n_units);
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Environments.WithImposedPrimitives.reset",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    auto sim = setup_sim_with_state(20UL);
    // pre check
    {
      auto check_container = [](auto const& container) {
        CHECK_FALSE(container.empty());
      };

      cpp17::apply(
          [=](auto const&... container) {
            EXPAND_PACK_LEFT_TO_RIGHT(check_container(container));
          },
          sim.imposed_primitives().data());
    }

    // argument doesnt matter
    CHECK_NOTHROW(sim.reset_module(elastica::Time{0.0}));

    // post
    {
      auto check_container = [](auto const& container) {
        CHECK(container.empty());
      };

      cpp17::apply(
          [=](auto const&... container) {
            EXPAND_PACK_LEFT_TO_RIGHT(check_container(container));
          },
          sim.imposed_primitives().data());
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Environments.WithImposedPrimitives.serialize",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    using namespace elastica::io;
    // NOTHROW case
    {
      auto sim = setup_sim_with_state(20UL);
      std::stringstream ss;  // any stream can be used
      // std::size_t n_elements(16UL);

      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive

        // modify the members a little
        sim.for_each_imposed_primitive<tmpl::list<MockImposedPrimitiveOne>>(
            [](auto& b) {
              // for minimal cosserat traits we have a vector
              std::get<0>(b) += 2;
            });

        CHECK_NOTHROW(sim.finalize());
        CHECK_NOTHROW(oarchive(sim));
      }

      {
        // from sim
        auto another_sim = setup_sim_with_state(20UL);
        BinaryInputArchive iarchive(ss);  //  Create an output archive

        // modify the members a little
        another_sim.for_each_imposed_primitive(make_overloader(
            [](MockImposedPrimitiveOne const& b) {
              // for minimal cosserat traits we have a vector
              CHECK(std::get<0UL>(b) == -5);
            },
            [](MockImposedPrimitiveTwo const& b) {
              CHECK(std::get<0UL>(b) == 'c');
            }));

        CHECK_NOTHROW(another_sim.finalize());
        CHECK_NOTHROW(iarchive(another_sim));

        // modify the members a little
        another_sim.for_each_imposed_primitive(make_overloader(
            [](MockImposedPrimitiveOne const& b) {
              // for minimal cosserat traits we have a vector
              CHECK(std::get<0UL>(b) == -3);
            },
            [](MockImposedPrimitiveTwo const& b) {
              CHECK(std::get<0UL>(b) == 'c');
            }));
      }
    }  // nothrow case
  }

}  // namespace