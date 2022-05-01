//
// Created by Tejaswin Parthasarathy on 4/1/22.
//

#pragma once

#include <vector>

#include "Framework/TestingFramework.hpp"
//
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactVectorAdapter.hpp"
//
#include "Simulator/Environments/Primitives.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Math/Identity.hpp"
#include "Utilities/Math/Vec3.hpp"

namespace {

  template <typename Response>
  void run_response_policy_test(Response& response) {
    // copied from resolver tests
    using namespace elastica::collision;
    using elastica::identity3;
    using elastica::iron;
    using elastica::real_t;
    using elastica::Vec3;

    Vec3 position{5.0, 3.0, 6.0};
    real_t radius(4.0);

    using P = SpherePrimitive;
    P first_sphere(0UL, 0UL, true, position, radius, iron);

    Vec3 velocity{1.0, 0.0, 0.0};
    Vec3 omega{0.0, 0.0, 0.0};
    first_sphere.transfer(position, velocity, omega);

    // collide many things against this primitive

    using Q = CapsulePrimitive;
    const real_t capsule_radius = radius;
    const real_t capsule_length = 2.0 * radius;
    const Vec3 capsule_position{position[0], position[1],
                                position[2] + radius + 1.75 * radius};

    Q capsule(1UL, 1UL, true, capsule_position, identity3(), capsule_radius,
              capsule_length, iron);
    capsule.transfer(capsule_position, identity3(), velocity, omega);

    using CV = ContactVectorAdapter<Contact>;
    std::vector<CV> batched_contact_vectors;
    // CV contacts;
    // add 3 batches
    {
      batched_contact_vectors.emplace_back();
      CV& contacts = batched_contact_vectors.back();

      // make up bullshit numbers here
      contacts.add_vertex_face_contact(&first_sphere, &capsule, Vec3{0., 0, 0.},
                                       Vec3{0., 0., 1.}, -0.2);
      contacts.add_vertex_face_contact(&first_sphere, &capsule, Vec3{0., 0, 0.},
                                       Vec3{0., 1., 0.}, -0.3);
      contacts.add_vertex_face_contact(&first_sphere, &capsule, Vec3{4., 0, 0.},
                                       Vec3{0., 1., 0.}, -0.4);
    }

    {
      batched_contact_vectors.emplace_back();
      CV& contacts = batched_contact_vectors.back();
      contacts.add_vertex_face_contact(&first_sphere, &capsule, Vec3{0., 4, 0.},
                                       Vec3{0., 1., 0.}, -0.5);
      contacts.add_vertex_face_contact(&first_sphere, &capsule, Vec3{0., 0, 4.},
                                       Vec3{0., 0., 1.}, -0.6);
      contacts.add_vertex_face_contact(&first_sphere, &capsule, Vec3{0., 0, 4.},
                                       Vec3{0., 0., 1.}, -0.3);
    }

    {
      batched_contact_vectors.emplace_back();
      CV& contacts = batched_contact_vectors.back();
      contacts.add_vertex_face_contact(&first_sphere, &capsule, Vec3{0., 4, 0.},
                                       Vec3{0., 1., 0.}, -0.3);
      contacts.add_vertex_face_contact(&first_sphere, &capsule, Vec3{1., 0, 4.},
                                       Vec3{0., 0., 1.}, -0.8);
      contacts.add_vertex_face_contact(&first_sphere, &capsule, Vec3{2., 0, 4.},
                                       Vec3{1., 0., 0.}, -0.5);
    }
    CHECK(::elastica::length(first_sphere.get_force()) == approx(0.0));
    CHECK(::elastica::length(capsule.get_force()) == approx(0.0));

    double max_penetration = response(batched_contact_vectors);
    CHECK(max_penetration == approx(0.8));

    CHECK_FALSE(::elastica::length(first_sphere.get_force()) == approx(0.0));
    CHECK_FALSE(::elastica::length(capsule.get_force()) == approx(0.0));
  }

}  // namespace
