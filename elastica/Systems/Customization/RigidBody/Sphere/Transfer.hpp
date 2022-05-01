//==============================================================================
/*!
//  \file
//  \brief Customization of transfer
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2020-2020 MattiaLab - All Rights Reserved
//
//  Distributed under the MIT License.
//  See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include "Simulator/Environments/Transfer/Transfer.hpp"  // refactor
//
#include "Simulator/Environments/Primitives/SpherePrimitive.hpp"  // refactor
//
#include "Systems/Customization/RigidBody/Sphere/PrimitiveTrait.hpp"
#include "Systems/Customization/RigidBody/common.hpp"  // for common traits
//
#include "Systems/RigidBody/Sphere.hpp"

namespace elastica {

  namespace customization {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct Transfer<::elastica::rigid_body::detail::SphereBlockSlice>
        : public ::elastica::collision::TransferTraits<
              ::elastica::rigid_body::detail::SphereBlockSlice> {
      // SystemType is an alias for MySystem
      // PrimitiveType is an alias for MyPrimitive
      // feel free to use them

      template <typename ConstPrimitiveIterator>
      static void do_transfer(to_t /* meta */, SystemType& collection,
                              from_t /* meta */, ConstPrimitiveIterator begin,
                              ConstPrimitiveIterator end) {
        std::for_each(
            begin, end,
            [&collection, index = 0UL](PrimitiveType const& primitive) mutable {
              collection.get_external_loads(index) += primitive.get_force();
              collection.get_external_torques(index) += primitive.get_torque();
              ++index;
            });
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct Transfer<::elastica::rigid_body::detail::SphereBlockConstSlice>
        : public ::elastica::collision::TransferTraits<
              ::elastica::rigid_body::detail::SphereBlockConstSlice> {
      template <typename PrimitiveIterator>
      static void do_transfer(from_t /* meta */, SystemType const& collection,
                              to_t /* meta */, PrimitiveIterator begin,
                              PrimitiveIterator end) {
        // Uses primitives transfer member
        std::for_each(
            begin, end,
            [&collection, index = 0UL](PrimitiveType& primitive) mutable {
              primitive.transfer(collection.get_radius(index),
                                 collection.get_position(index),
                                 collection.get_director(index),
                                 collection.get_velocity(index),
                                 collection.get_angular_velocity(index));
              ++index;
            });
      };
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace customization

}  // namespace elastica
