//
// Created by Tejaswin Parthasarathy on 3/8/22.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/Contact.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactType.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactVectorAdapter.hpp"
#include "Simulator/Environments/Primitives.hpp"
#include "Simulator/Materials.hpp"
#include "Utilities/AsConst.hpp"

namespace {

  using namespace elastica::collision;
  using ::elastica::Vec3;

#define STAMP_PRIMITIVES                                               \
  using Prim = SpherePrimitive;                                        \
  Vec3 first_position{0., 0., 0.};                                     \
  Prim first(0UL, 0UL, true, first_position, 1.0, elastica::iron);     \
                                                                       \
  Vec3 velocity{0., 0., 0.};                                           \
  Vec3 omega{0., 0., 0.};                                              \
  first.transfer(first_position, velocity, omega);                     \
                                                                       \
  Vec3 second_position{0., 0., 1.};                                    \
  Prim second(0UL, 1UL, true, second_position, 1.0, elastica::copper); \
  second.transfer(second_position, velocity, omega)

  // Because of static tracking

  ELASTICA_TEST_CASE("Unit.Environments.CollisionSystem.Contact.ContactVector",
                     "[Unit][Environments]") {
    using C = Contact;
    using Contacts = ContactVectorAdapter<C>;

    Contacts contacts;
    {
      STAMP_PRIMITIVES;

      contacts.add_vertex_face_contact(&first, &second, Vec3{0., 0., 0.5},
                                       Vec3{0., 0., 1.}, 0.5);

      CHECK(std::as_const(contacts[0UL]).get_index() == 0);
      CHECK(std::as_const(contacts[0UL]).get_first_body() == &first);
      CHECK(std::as_const(contacts[0UL]).get_second_body() == &second);
      CHECK(std::as_const(contacts[0UL]).get_position() == Vec3{0., 0., 0.5});
      CHECK(std::as_const(contacts[0UL]).get_normal() == Vec3{0., 0., 1.0});
      CHECK(std::as_const(contacts[0UL]).get_distance() == approx(0.5));
      // how to test?
      // check const access
      CHECK(std::as_const(contacts)[0UL].get_restitution() >= 0.0);
      CHECK(std::as_const(contacts)[0UL].get_stiffness() > 0.0);
      CHECK(std::as_const(contacts)[0UL].get_normal_damping() > 0.0);
      CHECK(std::as_const(contacts)[0UL].get_tangential_damping() > 0.0);
      CHECK(std::as_const(contacts)[0UL].get_friction() > 0.0);
    }

    {
      STAMP_PRIMITIVES;

      contacts.add_edge_edge_contact(&first, &second, Vec3{0., 0.5, 0.},
                                     Vec3{0., 1., 0.}, Vec3{0., 0., 1.},
                                     Vec3{0., 0., 1.}, 0.6);

      CHECK(std::as_const(contacts[1UL]).get_index() == 0);
      CHECK(std::as_const(contacts[1UL]).get_first_body() == &first);
      CHECK(std::as_const(contacts[1UL]).get_second_body() == &second);
      CHECK(std::as_const(contacts[1UL]).get_position() == Vec3{0., 0.5, 0.});
      CHECK(std::as_const(contacts[1UL]).get_normal() == Vec3{0., 1., 0.});
      CHECK(std::as_const(contacts[1UL]).get_distance() == approx(0.6));
      // how to test?
      // check const access
      CHECK(std::as_const(contacts)[1UL].get_restitution() >= 0.0);
      // are not used i think
      //      CHECK(std::as_const(contacts)[1UL].get_stiffness() > 0.0);
      //      CHECK(std::as_const(contacts)[1UL].get_normal_damping() > 0.0);
      //      CHECK(std::as_const(contacts)[1UL].get_tangential_damping() >
      //      0.0);
      CHECK(std::as_const(contacts)[1UL].get_friction() > 0.0);

      // also check the previous
      CHECK(std::as_const(contacts[0UL]).get_index() == 0);
      // Don't have the references no more, technically UB
      //      CHECK(std::as_const(contacts[0UL]).get_first_body() == &first);
      //      CHECK(std::as_const(contacts[0UL]).get_second_body() ==
      //      &second);
      CHECK(std::as_const(contacts[0UL]).get_position() == Vec3{0., 0., 0.5});
      CHECK(std::as_const(contacts[0UL]).get_normal() == Vec3{0., 0., 1.0});
      CHECK(std::as_const(contacts[0UL]).get_distance() == approx(0.5));
      // how to test?
      // check const access
      CHECK(std::as_const(contacts)[0UL].get_restitution() >= 0.0);
      CHECK(std::as_const(contacts)[0UL].get_stiffness() > 0.0);
      CHECK(std::as_const(contacts)[0UL].get_normal_damping() > 0.0);
      CHECK(std::as_const(contacts)[0UL].get_tangential_damping() > 0.0);
      CHECK(std::as_const(contacts)[0UL].get_friction() > 0.0);
    }

    contacts.clear();
    contacts.shrink_to_fit();
  }

}  // namespace
