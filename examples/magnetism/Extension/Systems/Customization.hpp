//==============================================================================
/*!
//  \file
//  \brief Customization of transfer for MagneticCosseratRod
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <elastica/Systems/Customization/CosseratRods.hpp>
//
//
#include "Extension/Systems/CosseratRods/CosseratRods.hpp"

namespace elastica {

  namespace customization {
// Assume for Millipede case study that transfer is not required as the primitive
// is no such type. This behavior is encoded for the MagneticFieldMillipede case
// in its customization file.

#define CONTACT_REQUIRED_FOR_MAGNETIC_RODS 0

#if CONTACT_REQUIRED_FOR_MAGNETIC_RODS
    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct Transfer<
        ::elastica::cosserat_rod::detail::MagnetizedCosseratRodBlockSlice>
        : public ::elastica::collision::TransferTraits<
              ::elastica::cosserat_rod::detail::
                  MagnetizedCosseratRodBlockSlice> {
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
    struct Transfer<::elastica::cosserat_rod::detail::
                        MagnetizedCosseratRodWithoutDampingBlockSlice>
        : public ::elastica::collision::TransferTraits<
              ::elastica::cosserat_rod::detail::
                  MagnetizedCosseratRodWithoutDampingBlockSlice> {
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
    struct Transfer<
        ::elastica::cosserat_rod::detail::MagnetizedCosseratRodBlockConstSlice>
        : public ::elastica::collision::TransferTraits<
              ::elastica::cosserat_rod::detail::
                  MagnetizedCosseratRodBlockConstSlice> {
      template <typename PrimitiveIterator>
      static void do_transfer(from_t from, SystemType const& collection,
                              to_t to, PrimitiveIterator begin,
                              PrimitiveIterator end) {
        // goes over number of elements in this case
        ::elastica::cosserat_rod::detail::transfer_cosserat_rod(
            from, collection, to, begin, end);
      }
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct Transfer<::elastica::cosserat_rod::detail::
                        MagnetizedCosseratRodWithoutDampingBlockConstSlice>
        : public ::elastica::collision::TransferTraits<
              ::elastica::cosserat_rod::detail::
                  MagnetizedCosseratRodWithoutDampingBlockConstSlice> {
      template <typename PrimitiveIterator>
      static void do_transfer(from_t from, SystemType const& collection,
                              to_t to, PrimitiveIterator begin,
                              PrimitiveIterator end) {
        // goes over number of elements in this case
        ::elastica::cosserat_rod::detail::transfer_cosserat_rod(
            from, collection, to, begin, end);
      }
    };
    /*! \endcond */
    //**************************************************************************
#endif

#undef CONTACT_REQUIRED_FOR_MAGNETIC_RODS

  }  // namespace customization

}  // namespace elastica
