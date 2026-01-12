//
// Created by Tejaswin Parthasarathy on 4/7/22.
//

#include <algorithm>
#include <cmath>
#include <functional>
#include <random>

#include "Framework/TestingFramework.hpp"
#include "ModuleSettings/Collisions.hpp"
#include "Simulator/Environments/CollisionSystem/Batches/BatchContainerTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactContainerTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Response/DEMForces/LinearSpringDashpot.hpp"
#include "Simulator/Environments/CollisionSystem/Response/DEMSolver.hpp"
#include "Simulator/Environments/Primitives.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/Geometry/Angles.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/Math/Rot3.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/Size.hpp"

// all

namespace {

  template <class iterator_type>
  class combination_generator {
    iterator_type first, last;
    std::vector<bool> use;
    unsigned r;
    typedef
        typename std::iterator_traits<iterator_type>::value_type element_type;

   public:
    combination_generator(iterator_type first_, iterator_type last_,
                          unsigned r_)
        : first(first_), last(last_), r(r_) {
      use.resize(std::distance(first, last), false);
      if (r > use.size())
        throw std::domain_error(
            "can't select more elements than exist for combination");
      std::fill(use.begin(), use.begin() + r, true);
    }
    template <class output_iterator>
    bool operator()(output_iterator result) {
      iterator_type c = first;
      const std::size_t s = use.size();
      for (unsigned i = 0; i < s; ++i, ++c) {
        if (use[i])
          (*result = *c, ++result);
      }
      return std::prev_permutation(use.begin(), use.end());
    }
  };

  template <class iterator_type>
  combination_generator<iterator_type> make_combination_generator(
      iterator_type first, iterator_type last, unsigned r) {
    return combination_generator<iterator_type>(first, last, r);
  }

}  // namespace
namespace {
  using elastica::iron;
  using elastica::real_t;
  using ::elastica::Rot3;
  using elastica::Vec3;
  using ::elastica::geometry::deg2rad;

  template <typename ExecutionPolicy_>
  struct TestConfig {
    using ForceModel = elastica::collision::response::LinearSpringDashpot;
    // need to have a better name
    struct KernelExecutionPolicy {
      using ExecutionPolicy = ExecutionPolicy_;
      // will also contain other static functions
    };
  };

  ELASTICA_TEST_CASE("Unit.Environments.CollisionResponse.DEMSolver",
                     "[Unit][Environments]") {
    // individual components have been tested, only test integrity here
    SECTION("Contact response integrity") {
      using BaselinePolicy = ::elastica::configuration::sequential_policy;
      using BaselineConfig = TestConfig<BaselinePolicy>;
      using BaselineAlgorithm =
          elastica::collision::response::DEMSolver<BaselineConfig, void, void>;
      using BaselineContacts =
          typename elastica::collision::ContactContainerTrait<
              BaselinePolicy>::type;
      using BaselineBatches =
          typename elastica::collision::batches::BatchContainerTrait<
              BaselinePolicy>::type;

      ////////////// start setup

      // copy from test_unionfind
      using ::elastica::collision::Contact;
      std::tuple<std::vector<elastica::collision::SpherePrimitive>,
                 std::vector<elastica::collision::CapsulePrimitive>>
          primitive_storage;

      static constexpr std::size_t sphere_tuple_index = 0UL;
      static constexpr std::size_t capsule_tuple_index = 1UL;

      // copy from test_unionfind
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
          std::uniform_real_distribution<real_t> polar_angle_dist(
              deg2rad(0.0), deg2rad(360.0));
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
      // set equal because contact indices are generated accordingly
      const std::size_t n_capsules(n_spheres);
      setup_integrity_test(n_spheres, n_capsules);

      // These generate indices for the entire contact batch
      using IndicesAlreadySeen = std::tuple<std::unordered_set<std::size_t>,
                                            std::unordered_set<std::size_t>>;
      using UniqueIndices =
          std::tuple<std::vector<std::size_t>, std::vector<std::size_t>>;
      IndicesAlreadySeen indices_already_seen;

      auto generate_unique_indices_for_a_single_batch =
          [&](const std::size_t n_spheres_in_one_batch,
              const std::size_t n_capsules_in_one_batch) -> UniqueIndices {
        std::random_device rd;
        std::mt19937 sphere_index_gen(rd());
        std::mt19937 capsule_index_gen(rd());
        std::uniform_int_distribution<std::size_t> sphere_index_dist(
            0, n_spheres - 1UL);
        std::uniform_int_distribution<std::size_t> capsule_index_dist(
            0, n_capsules - 1UL);

        UniqueIndices unique_indices;

        {
          auto& already_seen =
              std::get<sphere_tuple_index>(indices_already_seen);
          auto& buffer = std::get<sphere_tuple_index>(unique_indices);
          std::size_t n_added(0UL);

          do {
            auto potential_index = sphere_index_dist(sphere_index_gen);
            if (already_seen.find(potential_index) == already_seen.end()) {
              buffer.emplace_back(potential_index);
              already_seen.insert(potential_index);
              ++n_added;
            }
          } while (n_added < n_spheres_in_one_batch);
        }

        {
          auto& already_seen =
              std::get<capsule_tuple_index>(indices_already_seen);
          auto& buffer = std::get<capsule_tuple_index>(unique_indices);
          std::size_t n_added(0UL);

          do {
            auto potential_index = capsule_index_dist(capsule_index_gen);
            if (already_seen.find(potential_index) == already_seen.end()) {
              buffer.emplace_back(potential_index);
              already_seen.insert(potential_index);
              ++n_added;
            }
          } while (n_added < n_capsules_in_one_batch);
        }

        return unique_indices;
      };

      constexpr std::size_t n_elements_in_one_batch = 5UL;
      constexpr std::size_t n_spheres_in_one_batch = 3UL;
      constexpr std::size_t n_capsules_in_one_batch =
          n_elements_in_one_batch - n_spheres_in_one_batch;

      auto generate_unique_indices_for_a_batch = [&]() {
        return generate_unique_indices_for_a_single_batch(
            n_spheres_in_one_batch, n_capsules_in_one_batch);
      };

      // These generate indices for the entire contact batch
      auto generate_contact_batch_indices = [&](const std::size_t n_batches) {
        using UniqueIndicesCollection = std::vector<UniqueIndices>;

        UniqueIndicesCollection unique_indices_collection;
        unique_indices_collection.reserve(n_batches);

        // Form a connection between indices
        std::generate_n(std::back_inserter(unique_indices_collection),
                        n_batches, generate_unique_indices_for_a_batch);

        return unique_indices_collection;
      };

      auto check_each_indices_integrity = [=](auto& indices) {
        CHECK(std::size(std::get<sphere_tuple_index>(indices)) ==
              n_spheres_in_one_batch);
        CHECK(std::size(std::get<capsule_tuple_index>(indices)) ==
              n_capsules_in_one_batch);
      };

      auto check_indices_integrity = [=](auto& indices_collection) {
        for (auto& indices : indices_collection)
          check_each_indices_integrity(indices);
      };

      const std::size_t n_batches = 15UL;
      auto unique_indices = generate_contact_batch_indices(n_batches);
      CHECK(std::size(unique_indices) == n_batches);
      check_indices_integrity(unique_indices);

      auto n_choose_k = [](unsigned n, unsigned k) -> unsigned {
        if (k > n)
          return 0;
        if (k * 2 > n)
          k = n - k;
        if (k == 0)
          return 1;

        unsigned result = n;
        for (unsigned i = 2; i <= k; ++i) {
          result *= (n - i + 1);
          result /= i;
        }
        return result;
      };

      // with these unique indices, we now setup contacts and batches
      auto setup_contacts = [&](auto& contacts) {
        constexpr std::size_t n_combinations(2UL);
        std::array<std::size_t, n_combinations> buffer;
        auto it = buffer.begin();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<real_t> min_penetration_dist(-4.0, -1.0);
        real_t min_dist = 1000.0;

        std::uniform_real_distribution<real_t> polar_angle_dist(deg2rad(0.0),
                                                                deg2rad(360.0));
        std::uniform_real_distribution<real_t> inplane_angle_dist(
            deg2rad(0.0), deg2rad(180.0));

        contacts.clear();  // in case
        // I resize here based on some logic. Using this means
        // I don't need to refactor adding batches into a separate function
        // after adding all contacts. This is because push_back() on a vector
        // potentially reallocates, hence we may store references to null
        // objects in our batches which are just fancy references to the
        // contacts themselves
        auto n_contacts_in_one_set =
            n_choose_k(n_spheres_in_one_batch, n_combinations) +
            n_choose_k(n_capsules_in_one_batch, n_combinations) +
            n_spheres_in_one_batch * n_capsules_in_one_batch;
        auto total_n_contacts = n_contacts_in_one_set * n_batches;
        contacts.reserve(total_n_contacts);

        auto generate_dist = [&]() -> real_t {
          real_t generated_value = min_penetration_dist(gen);
          min_dist = std::min(min_dist, generated_value);
          return generated_value;
        };
        auto generate_normal = [&]() -> Vec3 {
          const real_t polar_angle = polar_angle_dist(gen);
          const real_t inplane_angle = inplane_angle_dist(gen);
          return {std::sin(polar_angle) * std::cos(inplane_angle),
                  std::sin(polar_angle) * std::sin(inplane_angle),
                  std::cos(polar_angle)};
        };

        // register contacts between all of them
        for (std::size_t batch_idx = 0UL; batch_idx < n_batches; ++batch_idx) {
          const auto& indices = unique_indices[batch_idx];
          const auto& sphere_indices = std::get<sphere_tuple_index>(indices);

          // make sphere-sphere combinations
          {
            auto combinations = make_combination_generator(
                sphere_indices.begin(), sphere_indices.end(), n_combinations);

            while (combinations(it)) {
              contacts.add_vertex_face_contact(
                  &std::get<sphere_tuple_index>(
                      primitive_storage)[std::get<0UL>(buffer)],
                  &std::get<sphere_tuple_index>(
                      primitive_storage)[std::get<1UL>(buffer)],
                  Vec3{0., 0, 0.}, generate_normal(), generate_dist());

              // need to manually reset :(
              it = buffer.begin();
            }

            // buffer gets populated after last, so it needs to be added
            contacts.add_vertex_face_contact(
                &std::get<sphere_tuple_index>(
                    primitive_storage)[std::get<0UL>(buffer)],
                &std::get<sphere_tuple_index>(
                    primitive_storage)[std::get<1UL>(buffer)],
                Vec3{0., 0, 0.}, generate_normal(), generate_dist());
          }

          const auto& capsule_indices = std::get<capsule_tuple_index>(indices);

          // make-capsule capsule combinations
          {
            auto combinations = make_combination_generator(
                capsule_indices.begin(), capsule_indices.end(), n_combinations);

            while (combinations(it)) {
              contacts.add_vertex_face_contact(
                  &std::get<capsule_tuple_index>(
                      primitive_storage)[std::get<0UL>(buffer)],
                  &std::get<capsule_tuple_index>(
                      primitive_storage)[std::get<1UL>(buffer)],
                  Vec3{0., 0, 0.}, generate_normal(), generate_dist());

              // need to manually reset :(
              it = buffer.begin();
            }

            // buffer gets populated after last, so it needs to be added
            contacts.add_vertex_face_contact(
                &std::get<capsule_tuple_index>(
                    primitive_storage)[std::get<0UL>(buffer)],
                &std::get<capsule_tuple_index>(
                    primitive_storage)[std::get<1UL>(buffer)],
                Vec3{0., 0, 0.}, generate_normal(), generate_dist());
          }

          // make sphere capsule combinations
          {
            for (auto sphere_idx : sphere_indices) {
              for (auto capsule_idx : capsule_indices) {
                contacts.add_vertex_face_contact(
                    &std::get<sphere_tuple_index>(
                        primitive_storage)[sphere_idx],
                    &std::get<capsule_tuple_index>(
                        primitive_storage)[capsule_idx],
                    Vec3{0., 0, 0.}, generate_normal(), generate_dist());
              }
            }
          }  // end sphere-capsule
        }

        return min_dist;
      };

      auto setup_batches = [&](auto& contacts, auto& batches) {
        const auto s = std::size(contacts);
        const auto n_contacts_in_one_set =
            s / n_batches;  // this has to be integral
        const auto stride = n_contacts_in_one_set;

        batches.clear();
        batches.resize(n_batches);

        for (std::size_t batch_idx = 0UL; batch_idx < n_batches; ++batch_idx) {
          auto& batch = batches[batch_idx];
          // and in each batch, push back the n_contact entries
          for (std::size_t lidx = 0UL; lidx < stride; ++lidx) {
            auto contact_idx = batch_idx * stride + lidx;
            batch.push_back(contacts[contact_idx]);
          }
        }
      };
      // end setup

      BaselineContacts baseline_contacts;
      BaselineBatches baseline_batches;
      // baseline_batches.resize(n_batches);

      auto expected_min_dist = setup_contacts(baseline_contacts);
      setup_batches(baseline_contacts, baseline_batches);

      auto record = [](auto f) {
        using RT = std::vector<Vec3>;
        return [f](auto const& primitives) -> RT {
          RT buffer{};
          buffer.reserve(std::size(primitives));
          std::transform(std::cbegin(primitives), std::cend(primitives),
                         std::back_inserter(buffer), f);
          return buffer;
        };
      };
      auto apply_on_primitives = [&](auto f) {
        return [&, f]() {
          return std::apply(
              [=](auto&... primitive_containers) {
                return std::make_tuple(f(primitive_containers)...);
              },
              primitive_storage);
        };
      };
      auto record_forces = record([](auto& prim) { return prim.get_force(); });
      auto get_forces = apply_on_primitives(record_forces);

      auto record_torques =
          record([](auto& prim) { return prim.get_torque(); });
      auto get_torques = apply_on_primitives(record_torques);

      auto clear_forces_from_primitive_buffer = [](auto& primitives) {
        // can do range based for loop
        std::for_each(std::begin(primitives), std::end(primitives),
                      [](auto& p) {
                        // Teja why you stupid?
                        p.reset_forces_and_torques();
                      });
      };

      auto clear_forces_from_primitives = [&]() {
        std::apply(
            [=](auto&... primitive_containers) {
              EXPAND_PACK_LEFT_TO_RIGHT(
                  clear_forces_from_primitive_buffer(primitive_containers));
            },
            primitive_storage);
      };

      clear_forces_from_primitives();

      /// // check for me
      auto is_null_sum = [](auto&& totals) {
        auto accumulate_from = [](auto const& force_buffer) {
          return std::accumulate(std::begin(force_buffer),
                                 std::end(force_buffer), Vec3{});
        };
        auto force_buffers = std::apply(
            [=](auto&&... buffers) {
              return std::make_tuple(accumulate_from(buffers)...);
            },
            std::forward<decltype(totals)>(totals));

        auto check_zero = [](Vec3 f) -> bool {
          return (f == Vec3{0., 0., 0.});
        };
        return check_zero(std::get<sphere_tuple_index>(force_buffers) +
                          std::get<capsule_tuple_index>(force_buffers));
        // check_net_zero_force(std::get<0UL>(forces_and_torques));  // only
        // forces
      };

      {
        auto baseline_forces = get_forces();
        auto baseline_torques = get_torques();
        // reduce
        CHECK(is_null_sum(baseline_forces));
        CHECK(is_null_sum(baseline_torques));
      }

      BaselineAlgorithm response{BaselineConfig::ForceModel{}};

      auto baseline_max_penetration = response(baseline_batches);
      // for comparison later on
      auto baseline_forces = get_forces();
      auto baseline_torques = get_torques();
      {
        // reduce
        CHECK(is_null_sum(baseline_forces));
        // need not be null
        // check_null_sum(baseline_torques);
      }

      CHECK(baseline_max_penetration == -expected_min_dist);

      // Now run all other algorithms and check against the baseline cache
      using Configuration = tmpl::list<
          // ::elastica::configuration::sequential_policy
          ::elastica::configuration::parallel_policy
          // ::elastica::configuration::hybrid_policy
          >;

      tmpl::for_each<Configuration>([&](auto config_type) {
        using PolicyType = tmpl::type_from<decltype(config_type)>;

        using ConfigType = TestConfig<PolicyType>;
        using CheckAlgorithm =
            elastica::collision::response::DEMSolver<ConfigType, void, void>;
        using CheckContacts =
            typename elastica::collision::ContactContainerTrait<
                PolicyType>::type;
        using CheckBatches =
            typename elastica::collision::batches::BatchContainerTrait<
                PolicyType>::type;
        SECTION(::pretty_type::get_name<ConfigType>()) {
          clear_forces_from_primitives();
          {
            REQUIRE(is_null_sum(get_forces()));
            REQUIRE(is_null_sum(get_torques()));
          }

          CheckContacts check_contacts;
          CheckBatches check_batches;
          // check_batches.resize(n_batches);

          /*auto check_min_dist =*/
          // setup_contacts_and_batches(check_contacts, check_batches);
          // auto expected_min_dist = setup_contacts(baseline_contacts);
          setup_batches(baseline_contacts, check_batches);

          CheckAlgorithm check_algorithm{typename ConfigType::ForceModel{}};

          auto actual_max_penetration = check_algorithm(check_batches);
          // for comparison later on
          auto check_forces = get_forces();
          auto check_torques = get_torques();

          CHECK(actual_max_penetration == baseline_max_penetration);
          CHECK(is_null_sum(check_forces));

          CHECK(check_forces == baseline_forces);
          CHECK(check_torques == baseline_torques);
        }
      });
    }
  }

}  // namespace
