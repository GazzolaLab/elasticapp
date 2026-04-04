//==============================================================================
/*!
//  \file
//  \brief Customization of transfer for Criss/Cross CosseratRod
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

#include <elastica/Simulator/Environments/GravityField.hpp>       // refactor
#include <elastica/Simulator/Environments/Transfer/Transfer.hpp>  // refactor
//
#include <elastica/Simulator/Environments/Primitives/CapsuleFromRodPrimitive.hpp>  // refactor
//
#include <elastica/Systems/Customization/CosseratRods/Geometry/PrimitiveTrait.hpp>
#include <elastica/Systems/Customization/CosseratRods/Transfer/Common.hpp>
#include <elastica/Systems/Customization/CosseratRods/common.hpp>  // for common traits
//
#include <elastica/Systems/CosseratRods/Block.hpp>
//
#include <elastica/Utilities/NoSuchType.hpp>
//
#include "Extension/Systems/CosseratRods.hpp"
#include "Types.hpp"

namespace elastica {

  namespace customization {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct PrimitiveTrait<
        ::elastica::cosserat_rod::detail::MagnetizedCosseratRodWithoutDamping> {
      using type = NoSuchType;
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct PrimitiveTrait<::elastica::cosserat_rod::detail::
                              MagnetizedCosseratRodWithoutDampingBlock>
        : public PrimitiveTrait<::elastica::cosserat_rod::detail::
                                    MagnetizedCosseratRodWithoutDamping> {};
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct PrimitiveTrait<::elastica::cosserat_rod::detail::
                              MagnetizedCosseratRodWithoutDampingBlockSlice>
        : public PrimitiveTrait<::elastica::cosserat_rod::detail::
                                    MagnetizedCosseratRodWithoutDamping> {};
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct PrimitiveTrait<
        ::elastica::cosserat_rod::detail::
            MagnetizedCosseratRodWithoutDampingBlockConstSlice>
        : public PrimitiveTrait<::elastica::cosserat_rod::detail::
                                    MagnetizedCosseratRodWithoutDamping> {};
    /*! \endcond */
    //**************************************************************************

  }  // namespace customization

}  // namespace elastica

namespace elastica {

  namespace customization {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct Transfer<
        ::elastica::cosserat_rod::detail::BackBoneCrissRodBlockSlice>
        : public ::elastica::collision::TransferTraits<
              ::elastica::cosserat_rod::detail::BackBoneCrissRodBlockSlice> {
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
        ::elastica::cosserat_rod::detail::BackBoneCrissRodBlockConstSlice>
        : public ::elastica::collision::TransferTraits<
              ::elastica::cosserat_rod::detail::
                  BackBoneCrissRodBlockConstSlice> {
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
    struct Transfer<
        ::elastica::cosserat_rod::detail::BackBoneCrossRodBlockSlice>
        : public ::elastica::collision::TransferTraits<
              ::elastica::cosserat_rod::detail::BackBoneCrossRodBlockSlice> {
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
        ::elastica::cosserat_rod::detail::BackBoneCrossRodBlockConstSlice>
        : public ::elastica::collision::TransferTraits<
              ::elastica::cosserat_rod::detail::
                  BackBoneCrossRodBlockConstSlice> {
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

  }  // namespace customization

}  // namespace elastica

namespace elastica {

  namespace environments {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <typename Traits>
    void resolve(elastica::environments::GravityField<Traits> const&,
                 elastica::cosserat_rod::detail::BackBoneCrissRodBlock&) {}
    template <typename Traits>
    void resolve(elastica::environments::GravityField<Traits> const&,
                 elastica::cosserat_rod::detail::BackBoneCrossRodBlock&) {}
    /*! \endcond */
    //**************************************************************************

  }  // namespace environments

}  // namespace elastica
