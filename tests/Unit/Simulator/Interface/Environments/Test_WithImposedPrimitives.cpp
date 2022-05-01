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
#include "Utilities/Overloader.hpp"
#include "Utilities/ProtocolHelpers.hpp"
/// serializatoin
#include "Simulator/IO/Serialization.hpp"
#include "Simulator/Interface/Environments/Serialize.hpp"

namespace mock {

  template <typename Traits>
  class WithImposedPrimitivesSimulator
      : public ::elastica::interface::environments::WithImposedPrimitives<
            Traits, WithImposedPrimitivesSimulator<Traits>> {
   public:
    using P = ::elastica::interface::environments::WithImposedPrimitives<
        Traits, WithImposedPrimitivesSimulator<Traits>>;
    WithImposedPrimitivesSimulator(bool arg_is_final = false)
        : P(), is_final(arg_is_final) {}
    using typename P::ImposedPrimitives;
    using typename P::ListOfAdmissibleImposedPrimitives;
    using TimeType = typename Traits::TimeType;

    // before
    using P::check_if_admissible;

    using P::imposed_primitives;

    using P::reset_module;

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
    return Sim();
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
    using ListOfImposedPrimitives =
        tmpl::list<MockImposedPrimitiveOne, MockImposedPrimitiveTwo>;
    auto sim = make_imposed_systems_simulator<ListOfImposedPrimitives>();
    (void)(sim.get_invalid_primitive_ref<MockImposedPrimitiveOne>());
    (void)(sim.get_invalid_primitive_ref<MockImposedPrimitiveTwo>());
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

    auto imposed_primitive_ref =
        sim.get_invalid_primitive_ref<MockImposedPrimitiveOne>();
    auto another_imposed_primitive_ref =
        sim.get_invalid_primitive_ref<MockImposedPrimitiveTwo>();
    SECTION("Emplace back correctness") {
      imposed_primitive_ref =
          sim.emplace_back_imposed<MockImposedPrimitiveOne>(-1, 127U);

      CHECK(std::get<int>(imposed_primitive_ref.get()) == -1);
      CHECK(std::get<std::uint16_t>(imposed_primitive_ref.get()) == 127U);
      CHECK(std::get<int>(cpp17::as_const(imposed_primitive_ref).get()) == -1);
      CHECK(std::get<std::uint16_t>(
                cpp17::as_const(imposed_primitive_ref).get()) == 127U);

      another_imposed_primitive_ref =
          sim.emplace_back_imposed<MockImposedPrimitiveTwo>('a', 4.525);
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
          sim.emplace_back_imposed<MockImposedPrimitiveOne>(-1, 127U);
      another_imposed_primitive_ref =
          sim.emplace_back_imposed<MockImposedPrimitiveTwo>('a', 4.525);

      for (std::size_t i = 0UL; i < 20UL; ++i) {
        sim.emplace_back_imposed<MockImposedPrimitiveOne>(
            -1 + int(i), 127U + std::uint8_t(i));
        sim.emplace_back_imposed<MockImposedPrimitiveTwo>(
            char(i), 4.525 + double(2 * i));
      }

      CHECK(std::get<int>(imposed_primitive_ref.get()) == -1);
      CHECK(std::get<std::uint16_t>(imposed_primitive_ref.get()) == 127U);

      CHECK(std::get<char>(another_imposed_primitive_ref.get()) == 'a');
      CHECK(std::get<double>(another_imposed_primitive_ref.get()) ==
            approx(4.525));
    }
    SECTION("Emplace back throws exception after finalize") {
      sim.finalize();
      CHECK_THROWS_WITH(
          sim.emplace_back_imposed<MockImposedPrimitiveTwo>('b', 234.2),
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
        sim.emplace_back_imposed<P>(std::get<0UL>(initializer),
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