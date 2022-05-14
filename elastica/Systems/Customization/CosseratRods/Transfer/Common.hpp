//==============================================================================
/*!
//  \file
//  \brief Customization of transfer for all CosseratRod types
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

#include "Simulator/Environments/Transfer/Transfer.hpp"      // refactor
#include "Simulator/Environments/Transfer/TransferTags.hpp"  // refactor
//
#include "Systems/Customization/CosseratRods/Geometry/PrimitiveTrait.hpp"
//
#include "Utilities/Math/Orthogonal.hpp"

namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      template <typename CosseratRodLikeSlice, typename ConstPrimitiveIterator>
      void transfer_cosserat_rod(::elastica::collision::tags::to_t /* meta */,
                                 CosseratRodLikeSlice& cosserat_rod_like_slice,
                                 ::elastica::collision::tags::from_t /* meta */,
                                 ConstPrimitiveIterator begin,
                                 ConstPrimitiveIterator end) {
        // goes over number of elements in this case
        std::for_each(begin, end,
                      [&collection = cosserat_rod_like_slice,
                       index = 0UL](auto const& primitive) mutable {
                        // distribute forces on both side of elements
                        collection.get_external_loads(index) +=
                            0.5 * primitive.get_force();
                        collection.get_external_loads(index + 1UL) +=
                            0.5 * primitive.get_force();

                        collection.get_external_torques(index) +=
                            primitive.get_torque();
                        ++index;
                      });
      }

      template <typename CosseratRodLikeConstSlice, typename PrimitiveIterator>
      void transfer_cosserat_rod(
          ::elastica::collision::tags::from_t /* meta */,
          CosseratRodLikeConstSlice const& cosserat_rod_like_const_slice,
          ::elastica::collision::tags::to_t /* meta */, PrimitiveIterator begin,
          PrimitiveIterator end) {
        // goes over number of elements in this case
        std::for_each(
            begin, end,
            [&collection = cosserat_rod_like_const_slice,
             index = 0UL](auto& primitive) mutable {
              primitive.transfer(
                  collection.get_element_dimension(index),
                  collection.get_element_length(index),
                  // Transform it here from expr to Vec3
                  // TODO : Fix later on
                  ::elastica::Vec3{0.5 *
                                   (collection.get_position(index) +
                                    collection.get_position(index + 1UL))},
                  // Warning : do NOT use directors directly here, as it results
                  // in a shear-wave instability arising from discrete-contact
                  // events, such as those occuring in rod-collision case study.
                  make_orthogonal_bases_from_tangent(
                      collection.get_tangent(index)),
                  // we scale it up by the mass to conserve total momentum
                  // while doing node 2 element
                  ::elastica::Vec3{(collection.get_mass(index) *
                                        collection.get_velocity(index) +
                                    collection.get_mass(index + 1UL) *
                                        collection.get_velocity(index + 1UL)) /
                                   (collection.get_mass(index) +
                                    collection.get_mass(index + 1UL))},
                  ::elastica::Vec3{collection.get_angular_velocity(index)});
              ++index;
            });
      }

    }  // namespace detail

  }  // namespace cosserat_rod

}  // namespace elastica