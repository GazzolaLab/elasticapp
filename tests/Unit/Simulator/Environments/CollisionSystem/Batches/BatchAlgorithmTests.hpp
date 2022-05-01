//
// Created by Tejaswin Parthasarathy on 4/2/22.
//

#pragma once

#include "Framework/TestingFramework.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactVectorAdapter.hpp"
//
#include "Simulator/Environments/CollisionSystem/Batches/Types.hpp"
//
#include "Simulator/Environments/Primitives.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Identity.hpp"
#include "Utilities/Math/Vec3.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TypeTraits/IsA.hpp"

namespace {

  template <typename Algorithm, typename Batches>
  void test_batch_algorithm(Algorithm& algorithm, Batches& batches) {
    using namespace elastica::collision;
    using elastica::identity3;
    using elastica::iron;
    using elastica::real_t;
    using elastica::Vec3;

    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);
    Vec3 velocity{1.0, 0.0, 0.0};
    Vec3 omega{0.0, 0.0, 0.0};

    using P = SpherePrimitive;
    const std::size_t n_spheres = 10UL;

    std::vector<P> spheres{};

    for (std::size_t i_sphere = 0UL; i_sphere < n_spheres; ++i_sphere) {
      spheres.emplace_back(0UL, 0UL, true, position, radius, iron);
      spheres.back().transfer(position, velocity, omega);
    }

    using Q = CapsulePrimitive;
    const std::size_t n_capsules = n_spheres;

    std::vector<Q> capsules{};

    const real_t capsule_radius = radius;
    const real_t capsule_length = 2.0 * radius;
    const Vec3 capsule_position{position[0], position[1],
                                position[2] + radius + 1.75 * radius};
    for (std::size_t i_capsule = 0UL; i_capsule < n_capsules; ++i_capsule) {
      capsules.emplace_back(1UL, 1UL, true, capsule_position, identity3(),
                            capsule_radius, capsule_length, iron);
      capsules.back().transfer(capsule_position, identity3(), velocity, omega);
    }

    // there are 15 contacts overall
    // each batch here has {C1, C2, C3}
    // C1 : sphere1 <-> capsule1
    // C2 : sphere2 <-> capsule2
    // C3 : capsule1 <-> capsule2

    using CV = ContactVectorAdapter<Contact>;
    CV contacts;

    // register all spheres to capsules first
    for (std::size_t i = 0UL; i < n_capsules; ++i) {
      contacts.add_vertex_face_contact(&spheres[i], &capsules[i],
                                       Vec3{0., 0, 0.}, Vec3{0., 0., 1.}, -0.2);
    }
    // register all capsule i to capsule (i + 1) next,
    for (std::size_t i = 0UL; i < (n_capsules / 2); ++i) {
      contacts.add_vertex_face_contact(&capsules[2 * i], &capsules[2 * i + 1],
                                       Vec3{0., 0, 0.}, Vec3{0., 0., 1.}, -0.2);
    }

    REQUIRE(elastica::size(batches) == 0);

    const auto n_contacts(elastica::size(contacts));

    algorithm(contacts, batches);

    const auto n_batches = elastica::size(batches);

    auto get_first_body = [](Contact& c) { return c.get_first_body(); };
    auto get_second_body = [](Contact& c) { return c.get_second_body(); };

    if (::tt::is_a_v<batches::SingleBatch, Algorithm>) {
      CHECK(n_batches == 1UL);

      // all contacts in batches
      CHECK(elastica::size(batches[0UL]) == n_contacts);

      // check some randomly
      for (auto ind : {0UL, n_capsules - 1UL}) {
        CHECK(get_first_body(batches[0UL][ind]) ==
              std::addressof(spheres[ind]));
        CHECK(get_second_body(batches[0UL][ind]) ==
              std::addressof(capsules[ind]));
      }

      for (auto ind : {0UL, 1UL}) {
        CHECK(get_first_body(batches[0UL][n_capsules + ind]) ==
              std::addressof(capsules[2 * ind]));
        CHECK(get_second_body(batches[0UL][n_capsules + ind]) ==
              std::addressof(capsules[2 * ind + 1UL]));
      }

    } else if (::tt::is_a_v<batches::UnionFind, Algorithm>) {
      CHECK(n_batches == n_contacts / 3UL);

      // now test batches by sequential looping
      for (auto& batch : batches) {
        // ensure each batch is of size 3
        CHECK(elastica::size(batch) == 3UL);
        // check batch is contact between
      }
    } else {
      CHECK(false);
    }
  }

}  // namespace
