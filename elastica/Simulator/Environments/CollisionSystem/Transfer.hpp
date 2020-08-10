#pragma once

//// Declare all types
#include "elastica/Simulator/Environments/CollisionSystem/Transfer/Types.hpp"
////
#include "elastica/Simulator/Environments/CollisionSystem/Transfer/TransferTraits.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Transfer/Transfer.hpp"
#include "elastica/Simulator/Environments/CollisionSystem/Transfer/TransferHelper.hpp"

//#include <algorithm>
//#include <iterator>
//#include <type_traits>
//
////#include <blaze/math/Math.h>
//
//#include "Simulator/Environments/CollisionSystem/TransferTags.hpp"
//#include "Simulator/Environments/Primitives/SpherePrimitive.hpp"
//#include "elastica/ErrorHandling/Assert.hpp"
//#include "elastica/Systems/RigidBodyBase.hpp"
//#include "elastica/Utilities/Math/Trans.hpp"
//
////#include <iostream>
//
//namespace elastica {
//
//  namespace collision {
//
//    // No need to specialize transfer interface for pointer types to use in TBB
//    // iterator traits simply work!
//    //  int *p = new int[50];
//    //  using my_type =typename std::iterator_traits<decltype(p)>::reference;
//    //  std::cout << typeid(my_type).name() << std::endl;
//
//    // TODO noexcept checking, restrict checking, GSL not null
//    // TODO const pointer and const iterator to prevent aliasing
//
//    // Interface functions
//    template <typename Collection, typename PrimitiveIterator>
//    inline void transfer(tags::from_t /* meta */,
//                         Collection const& collection, tags::to_t /* meta */,
//                         PrimitiveIterator first, PrimitiveIterator last) {
//      transfer(
//          collection, first, last,
//          std::is_same<
//              typename Collection::PrimitiveType,
//              typename std::iterator_traits<PrimitiveIterator>::value_type>{});
//    }
//
//    template <typename Collection, typename PrimitiveIterator>
//    inline void transfer(tags::to_t /* meta */, PrimitiveIterator first,
//                         PrimitiveIterator last, tags::from_t /* meta */,
//                         Collection const& collection) {
//      transfer(
//          collection, first, last,
//          std::is_same<
//              typename Collection::PrimitiveType,
//              typename std::iterator_traits<PrimitiveIterator>::value_type>{});
//    }
//
//    template <typename Collection, typename ConstPrimitiveIterator>
//    inline void transfer(tags::from_t /* meta */,
//                         ConstPrimitiveIterator first,
//                         ConstPrimitiveIterator last, tags::to_t /* meta */,
//                         Collection& collection) {
//      transfer(
//          first, last, collection,
//          std::is_same<std::add_const_t<typename Collection::PrimitiveType>,
//                       std::remove_reference_t<typename std::iterator_traits<
//                           ConstPrimitiveIterator>::reference>>{});
//    }
//
//    template <typename Collection, typename ConstPrimitiveIterator>
//    inline void transfer(tags::to_t /* meta */, Collection& collection,
//                         tags::from_t /* meta */,
//                         ConstPrimitiveIterator first,
//                         ConstPrimitiveIterator last) {
//      transfer(
//          first, last, collection,
//          std::is_same<std::add_const_t<typename Collection::PrimitiveType>,
//                       std::remove_reference_t<typename std::iterator_traits<
//                           ConstPrimitiveIterator>::reference>>{});
//    }
//
//    // Details
//    // TODO : Hide in impl? Harder to extend if thats the case
//    // Default implementation always false
//    //    template <typename Collection, typename PrimitiveIterator>
//    //    void transfer(Collection const& collection, PrimitiveIterator first,
//    //                  PrimitiveIterator last, std::false_type) {
//    //      static_assert(false, "Should never be called");
//    //    }
//
//    // Implementation functions
//    template <typename SphereIterator>
//    void transfer(::elastica::rigidbody::Sphere const& body_collection,
//                  SphereIterator first, SphereIterator last, std::true_type) {
//      ELASTICA_ASSERT(body_collection.size() == static_cast<std::size_t>(last - first), "Oops");
//      // TODO : Check if a local copy on stack of spheres members using
//      // transpose and then transfer to sphereiterator is faster
//      std::for_each(
//          first, last,
//          [&body_collection, index = 0UL](auto& sphere_primitive) mutable {
//            // TODO : Figure out a way to put in a message without losing speed
//            // See benchmark in sphere primitives transfer function gist
//            // Does a column slice, need to then convert to rows for primitives
//            sphere_primitive.transfer(
//                body_collection.get_radius(index),
//                (body_collection.get_position(index)),
//                (body_collection.get_velocity(index)),
//                (body_collection.get_angular_velocity(index)));
//            //            sphere_primitive.radius_ =
//            //            body_collection.get_radius(index);
//            //            sphere_primitive.gpos_ =
//            //            body_collection.get_position(index);
//            //            // sphere_primitive.rot_mat_ =
//            //            // body_collection.get_rotation_matrix(index);
//            //            sphere_primitive.v_ =
//            //            body_collection.get_velocity(index);
//            //            sphere_primitive.w_ =
//            //            body_collection.get_angular_velocity(index);
//            ++index;
//          });
//    }
//
//    // Reverse transfer functions
//    // Default implementation always false
//    //    template <typename Collection, typename ConstPrimitiveIterator>
//    //    void transfer(ConstPrimitiveIterator first, ConstPrimitiveIterator
//    //    last,
//    //                  Collection const& collection, std::false_type) {
//    //      static_assert(false, "Should never be called, try calling with
//    //      cbegin");
//    //    }
//
//    template <typename ConstSphereIterator>
//    void transfer(ConstSphereIterator first, ConstSphereIterator last,
//                  ::elastica::rigidbody::Sphere& sphere_collection,
//                  std::true_type) {
//      ELASTICA_ASSERT(
//          sphere_collection.size() == static_cast<std::size_t>(last - first),
//          "Oops");
//      // Friended
//      std::for_each(
//          first, last,
//          [&sphere_collection, index = 0UL](auto& sphere_primitive) mutable {
//            sphere_collection.get_force(index) += (sphere_primitive.get_force());
//            sphere_collection.get_torque(index) +=
//                (sphere_primitive.get_torque());
//            ++index;
//          });
//
////      std::cout << "Force : " << sphere_collection.get_force(0UL)[2] << " ";
////      std::cout << "Velocity: " << sphere_collection.get_velocity(0UL)[2] << " ";
////      std::cout << "Position: " << sphere_collection.get_position(0UL)[2] << " ";
//    }
//
//
//  }  // namespace collision
//
//}  // namespace elastica
