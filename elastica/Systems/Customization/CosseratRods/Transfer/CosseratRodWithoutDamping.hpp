//==============================================================================
/*!
//  \file
//  \brief Customization of transfer for CosseratRodWithoutDamping
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
#include "Simulator/Environments/Primitives/CapsuleFromRodPrimitive.hpp"  // refactor
//
#include "Systems/Customization/CosseratRods/Geometry/PrimitiveTrait.hpp"
#include "Systems/Customization/CosseratRods/Transfer/Common.hpp"
#include "Systems/Customization/CosseratRods/common.hpp"  // for common traits
//
#include "Systems/CosseratRods/CosseratRods.hpp"

namespace elastica {

  namespace customization {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct Transfer<
        ::elastica::cosserat_rod::detail::CosseratRodWithoutDampingBlockSlice>
        : public ::elastica::collision::TransferTraits<
              ::elastica::cosserat_rod::detail::
                  CosseratRodWithoutDampingBlockSlice> {
      // SystemType is an alias for MySystem
      // PrimitiveType is an alias for MyPrimitive
      // feel free to use them

      template <typename ConstPrimitiveIterator>
      static void do_transfer(to_t to, SystemType& collection, from_t from,
                              ConstPrimitiveIterator begin,
                              ConstPrimitiveIterator end) {
        // goes over number of elements in this case
        ::elastica::cosserat_rod::detail::transfer_cosserat_rod(
            to, collection, from, begin, end);
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct Transfer< ::elastica::cosserat_rod::detail::
                         CosseratRodWithoutDampingBlockConstSlice>
        : public ::elastica::collision::TransferTraits<
              ::elastica::cosserat_rod::detail::
                  CosseratRodWithoutDampingBlockConstSlice> {
      template <typename PrimitiveIterator>
      static void do_transfer(from_t from, SystemType const& collection,
                              to_t to, PrimitiveIterator begin,
                              PrimitiveIterator end) {
        // goes over number of elements in this case
        ::elastica::cosserat_rod::detail::transfer_cosserat_rod(
            from, collection, to, begin, end);
      };
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace customization

}  // namespace elastica
