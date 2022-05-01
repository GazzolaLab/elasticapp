//
// Created by Tejaswin Parthasarathy on 4/2/22.
//

#include <algorithm>
#include <functional>
#include <random>

#include "Framework/TestingFramework.hpp"
#include "ModuleSettings/Collisions.hpp"
#include "Simulator/Environments/CollisionSystem/Batches/BatchAlgorithmTests.hpp"
#include "Simulator/Environments/CollisionSystem/Batches/BatchContainerTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Batches/UnionFind.hpp"
#include "Simulator/Environments/CollisionSystem/Configuration.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactContainerTrait.hpp"
#include "Simulator/Environments/Primitives.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/Size.hpp"

namespace {
  using elastica::identity3;
  using elastica::iron;
  using elastica::real_t;
  using ::elastica::Rot3;
  using elastica::Vec3;
  using ::elastica::geometry::deg2rad;

  template <typename Config_>
  void test(std::true_type) {
    using Contact = elastica::collision::Contact;

    elastica::collision::batches::UnionFind<Config_> batch_algorithm{};
    using ContactRef = std::reference_wrapper<Contact>;
    using Batch = std::vector<ContactRef>;
    using Batches = std::vector<Batch>;
    Batches batches;
    test_batch_algorithm(batch_algorithm, batches);
  }

  template <typename Config_>
  void test(std::false_type) {
    SUCCEED("UnionFind not tested since ELASTICA_BATCH_GENERATOR is different");
  }

  template <typename Policy>
  struct UFConfig {
    using This = UFConfig<Policy>;
    template <typename C>
    using BatchGenerator = elastica::collision::batches::UnionFind<C>;
    using BodyType = elastica::collision::RigidBodyPrimitive;
    using BodyID = BodyType*;
    using ConstBodyID = BodyType const*;

    struct KernelExecutionPolicy {
      using ExecutionPolicy = Policy;
      // will also contain other static functions
    };
  };

  template <typename T>
  auto collect_pairs(T const& contact_batches) {
    using IndexPair = std::pair<std::size_t, std::size_t>;
    using BatchPair = std::set<IndexPair>;  // ordering is fucked up boi
    using RT = std::set<BatchPair>;
    RT rt{};
    using std::begin;
    using std::end;
    std::transform(
        begin(contact_batches), end(contact_batches),
        std::inserter(rt, std::begin(rt)), [&](auto const& contacts) {
          BatchPair batch_pair;
          std::transform(begin(contacts), end(contacts),
                         std::inserter(batch_pair, std::begin(batch_pair)),
                         [&](elastica::collision::Contact const& contact) {
                           return std::make_pair(
                               contact.get_first_body()->getSystemID(),
                               contact.get_second_body()->getSystemID());
                         });
          return batch_pair;
        });
    return rt;
  }

  template <typename BaselineConfig_>
  void test_integrity(std::true_type) {
    using ::elastica::collision::Contact;

    // test sequential, parallel
    using BaselineConfig = BaselineConfig_;
    using BaselinePolicy =
        typename BaselineConfig::KernelExecutionPolicy::ExecutionPolicy;
    using BaselineAlgorithm =
        typename BaselineConfig::template BatchGenerator<BaselineConfig>;

    BaselineAlgorithm baseline_algorithm{};

    using BaselineContacts =
        typename elastica::collision::ContactContainerTrait<
            BaselinePolicy>::type;
    using BaselineBatches =
        typename elastica::collision::batches::BatchContainerTrait<
            BaselinePolicy>::type;
    // using Batch = typename Batches::value_type;
    // using ContactRef = typename Batch::value_type;

    // I don't think this needs to be parallel as we purely have pointer to it
    std::tuple<std::vector<elastica::collision::SpherePrimitive>,
               std::vector<elastica::collision::CapsulePrimitive>>
        primitive_storage;

    static constexpr std::size_t sphere_tuple_index = 0UL;
    static constexpr std::size_t capsule_tuple_index = 1UL;

    auto setup_integrity_test = [&](const std::size_t n_spheres,
                                    const std::size_t n_capsules) {
      {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<real_t> position_dist(0.0, 1.0);
        std::uniform_real_distribution<real_t> radius_dist(1e-4, 5e-2);

        Vec3 velocity{1.0, 0.0, 0.0};
        Vec3 omega{0.0, 0.0, 0.0};
        for (std::size_t sphere_idx = 0UL; sphere_idx < n_spheres;
             ++sphere_idx) {
          Vec3 position{position_dist(gen), position_dist(gen),
                        position_dist(gen)};

          std::get<sphere_tuple_index>(primitive_storage)
              .emplace_back(sphere_idx, sphere_idx, true, position,
                            radius_dist(gen), iron);
          std::get<sphere_tuple_index>(primitive_storage)
              .back()
              .transfer(
                  /*new_radius, */ position, velocity, omega);
        }
      }
      {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<real_t> position_dist(0.0, 1.0);
        std::uniform_real_distribution<real_t> radius_dist(1e-4, 5e-2);
        std::uniform_real_distribution<real_t> length_dist(1e-4, 5e-2);
        std::uniform_real_distribution<real_t> polar_angle_dist(deg2rad(0.0),
                                                                deg2rad(360.0));
        std::uniform_real_distribution<real_t> inplane_angle_dist(
            deg2rad(0.0), deg2rad(180.0));

        Vec3 velocity{1.0, 0.0, 0.0};
        Vec3 omega{0.0, 0.0, 0.0};

        for (std::size_t capsule_idx = 0UL; capsule_idx < n_capsules;
             ++capsule_idx) {
          const real_t capsule_radius = radius_dist(gen);
          // i assume length is always greater than radius
          const real_t capsule_length = length_dist(gen) /*+ capsule_radius*/;
          const Vec3 capsule_position{position_dist(gen), position_dist(gen),
                                      position_dist(gen)};
          const real_t polar_angle = polar_angle_dist(gen);
          const real_t inplane_angle = inplane_angle_dist(gen);
          const Vec3 capsule_axis{
              std::sin(polar_angle) * std::cos(inplane_angle),
              std::sin(polar_angle) * std::sin(inplane_angle),
              std::cos(polar_angle)};
          std::get<capsule_tuple_index>(primitive_storage)
              .emplace_back(n_spheres + capsule_idx, n_spheres + capsule_idx,
                            true, capsule_position, capsule_axis,
                            capsule_radius, capsule_length, iron);
          const Rot3 rot(std::get<capsule_tuple_index>(primitive_storage)
                             .back()
                             .get_rotation_matrix());
          std::get<capsule_tuple_index>(primitive_storage)
              .back()
              .transfer(capsule_position, rot, velocity, omega);
        }
      }
    };
    const std::size_t n_spheres(100UL);
    const std::size_t n_capsules(
        n_spheres);  // set equal because contact indices are generated
    // accordingly
    setup_integrity_test(n_spheres, n_capsules);

    auto generate_contact_indices = [&](const std::size_t n_contacts) {
      std::random_device rd;
      std::mt19937 first_gen(rd());
      std::mt19937 second_gen(rd());
      std::uniform_int_distribution<std::size_t> first_index_dist(
          0, n_spheres - 1UL);
      std::uniform_int_distribution<std::size_t> second_index_dist(
          0, n_capsules - 1UL);
      using IndexPair = std::pair<std::size_t, std::size_t>;
      using IndexPairs = std::vector<IndexPair>;

      struct index_pair_hash {
        std::size_t operator()(const IndexPair& p) const {
          auto h1 = std::hash<std::size_t>{}(p.first);
          auto h2 = std::hash<std::size_t>{}(p.second);

          // Mainly for demonstration purposes, i.e. works but is overly
          // simple In the real world, use sth. like boost.hash_combine
          return h1 ^ h2;
        }
      };
      using IndexSet = std::unordered_set<IndexPair, index_pair_hash>;

      IndexSet index_set;

      // Form a connection between indices
      std::generate_n(std::inserter(index_set, std::begin(index_set)),
                      n_contacts, [&]() -> IndexPair {
                        // To ensure that (23, 32) and (32, 23) are registered
                        // as the same index
                        return std::minmax(first_index_dist(first_gen),
                                           second_index_dist(second_gen));
                      });

      return IndexPairs(std::begin(index_set), std::end(index_set));
    };

    const std::size_t n_contacts_each(
        50UL);  // out of 100 * 99 / 2 = 5050 possible pairings
    auto sphere_sphere_indices = generate_contact_indices(n_contacts_each);
    auto capsule_capsule_indices = generate_contact_indices(n_contacts_each);
    auto sphere_capsule_indices = generate_contact_indices(n_contacts_each);

    auto check_each_indices_integrity = [n_contacts_each](auto const& indices) {
      // it may have some redundancies but shouldn't be / 2
      REQUIRE(elastica::size(indices) >= n_contacts_each / 2);
    };
    auto check_indices_integrity = [&]() {
      check_each_indices_integrity(sphere_sphere_indices);
      check_each_indices_integrity(capsule_capsule_indices);
      check_each_indices_integrity(sphere_capsule_indices);
    };
    check_indices_integrity();

    auto setup_contacts = [&](auto& contacts) {
      // setup randomized contacts between
      // 1. two spheres
      // 2. two capsules
      // 3. spheres and capsules
      // and check the UUID we load each by

      // register all spheres to capsules first
      for (const auto& ss : sphere_sphere_indices) {
        contacts.add_vertex_face_contact(
            &std::get<sphere_tuple_index>(primitive_storage)[ss.first],
            &std::get<sphere_tuple_index>(primitive_storage)[ss.second],
            Vec3{0., 0, 0.}, Vec3{0., 0., 1.}, -0.2);
      }

      // register all capsule to capsule next,
      for (const auto& cc : capsule_capsule_indices) {
        contacts.add_vertex_face_contact(
            &std::get<capsule_tuple_index>(primitive_storage)[cc.first],
            &std::get<capsule_tuple_index>(primitive_storage)[cc.second],
            Vec3{0., 0, 0.}, Vec3{0., 0., 1.}, -0.2);
      }

      for (const auto& sc : sphere_capsule_indices) {
        contacts.add_vertex_face_contact(
            &std::get<sphere_tuple_index>(primitive_storage)[sc.first],
            &std::get<capsule_tuple_index>(primitive_storage)[sc.second],
            Vec3{0., 0, 0.}, Vec3{0., 0., 1.}, -0.2);
      }
    };

    BaselineContacts baseline_contacts;
    BaselineBatches baseline_batches;

    REQUIRE(elastica::size(baseline_batches) == 0);
    REQUIRE(elastica::size(baseline_contacts) == 0);

    auto there_are_batches_with_exactly_one_contact = [&]() -> bool {
      return (elastica::size(baseline_batches) ==
              (elastica::size(baseline_contacts)));
    };

    auto there_are_only_few_batches = [&]() -> bool {
      return (elastica::size(baseline_batches) < 5UL);
    };

    do {
      // regenerate indices again
      sphere_sphere_indices = generate_contact_indices(n_contacts_each);
      capsule_capsule_indices = generate_contact_indices(n_contacts_each);
      sphere_capsule_indices = generate_contact_indices(n_contacts_each);

      check_indices_integrity();

      baseline_contacts.clear();
      setup_contacts(baseline_contacts);

      baseline_batches.clear();
      baseline_algorithm(baseline_contacts, baseline_batches);
    } while (there_are_batches_with_exactly_one_contact() or
             there_are_only_few_batches());  // more robust test

    REQUIRE_FALSE(baseline_batches.empty());
    //      REQUIRE(cpp17::size(baseline_batches) == 4UL);

    // Now run all other algorithms and check against the baseline cache
    using Configuration =
        tmpl::list<  // ::elastica::configuration::sequential_policy,
            ::elastica::configuration::parallel_policy
            // ::elastica::configuration::hybrid_policy
            >;

    tmpl::for_each<Configuration>([&](auto config_type) {
      using PolicyType = tmpl::type_from<decltype(config_type)>;
      using ConfigType = UFConfig<PolicyType>;
      using CheckAlgorithm =
          typename ConfigType::template BatchGenerator<ConfigType>;
      using CheckBatches =
          typename elastica::collision::batches::BatchContainerTrait<
              PolicyType>::type;
      SECTION(::pretty_type::get_name<ConfigType>()) {
        CheckAlgorithm algorithm_to_be_checked{};

        // CheckBatches batch_to_be_setup;
        // setup_cache(cache_to_be_setup);

        CheckBatches batches_to_be_checked;

        algorithm_to_be_checked(baseline_contacts, batches_to_be_checked);

        auto actual(collect_pairs(batches_to_be_checked));
        auto expected(collect_pairs(baseline_batches));

        CHECK(actual == expected);
      }
    });
  }

  template <typename BaselineConfig_>
  void test_integrity(std::false_type) {
    SUCCEED("UnionFind not tested since ELASTICA_BATCH_GENERATOR is different");
  }

  ELASTICA_TEST_CASE("Unit.Environments.ContactBatches.UnionFind",
                     "[Unit][Environments]") {
    // TypeDisplayer<elastica::collision::CollisionConfig> td{};
    using BaselinePolicy = elastica::configuration::sequential_policy;
    using BaselineConfig = UFConfig<BaselinePolicy>;
    auto to_run_tests_or_not =
        tt::is_a<elastica::collision::batches::UnionFind,
                 ELASTICA_BATCH_GENERATOR<BaselineConfig>>{};
    SECTION("Sequential") {
      // weirdness stems from ELASTICA_BATCH_GENERATOR being anything
      test<BaselineConfig>(to_run_tests_or_not);
    }
    SECTION("UnionFind integrity") {
      test_integrity<BaselineConfig>(to_run_tests_or_not);
    }
  }

}  // namespace
