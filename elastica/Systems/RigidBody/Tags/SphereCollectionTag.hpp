#pragma once

#include <algorithm>

#include "elastica/ErrorHandling/Assert.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Transfer.hpp"
#include "elastica/Simulator/Environments/Primitives/Types.hpp"

//////////// Forward declarations come first
#include "Systems/RigidBody/Types.hpp"
////////////
#include "Systems/RigidBody/SphereCollection.hpp"

namespace elastica {


  // Note that one should not qualify the tags in this type list by a reference or
  // cv qulifiers for the systemtype and primitivetype, else a hard error is
  // thrown
  struct SphereCollectionTag {
    using SystemType = SphereCollection;
    using PrimitiveType = collision::SpherePrimitive;

    // temporary
    using SystemType = PrimitiveType;
  };

  namespace collision {

    template <>
    struct Transfer<SphereCollectionTag>
        : public TransferTraits<SphereCollectionTag> {
      template <typename PrimIter>
      static void do_transfer(from_t /* meta */,
                              SystemType const& collection,
                              to_t /* meta */, PrimIter start, PrimIter end) {
        ELASTICA_ASSERT(
            collection.size() == static_cast<std::size_t>(end - start), "Oops");
        // TODO : Check if a local copy on stack of spheres members using
        // transpose and then transfer to sphereiterator is faster
        std::for_each(
            start, end,
            [&collection, index = 0UL](auto& sphere_primitive) mutable {
              // TODO : Figure out a way to put in a message without losing
              // speed See benchmark in sphere primitives transfer function gist
              // Does a column slice, need to then convert to rows for
              // primitives
              sphere_primitive.transfer(
                  collection.get_radius(index),
                  (collection.get_position(index)),
                  (collection.get_velocity(index)),
                  (collection.get_angular_velocity(index)));
              ++index;
            });
      }

      template <typename ConstPrimIter>
      static void do_transfer(to_t /* meta */, SystemType& collection,
                              from_t /* meta */, ConstPrimIter start,
                              ConstPrimIter end) {
        ELASTICA_ASSERT(
            collection.size() == static_cast<std::size_t>(end - start), "Oops");
        // Friended
        std::for_each(
            start, end,
            [&collection, index = 0UL](auto& sphere_primitive) mutable {
              collection.get_force(index) += (sphere_primitive.get_force());
              collection.get_torque(index) += (sphere_primitive.get_torque());
              ++index;
            });

        //      std::cout << "Force : " << sphere_collection.get_force(0UL)[2]
        //      << " "; std::cout << "Velocity: " <<
        //      sphere_collection.get_velocity(0UL)[2] << " "; std::cout <<
        //      "Position: " << sphere_collection.get_position(0UL)[2] << " ";
      }
    };

  }  // namespace collision

}  // namespace elastica
