//==============================================================================
/*!
//  \file
//  \brief Serialization support for the Primitives module
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
///
#include "Simulator/Environments/Primitives/Types.hpp"
///
#include "Simulator/Environments/Primitives/PrimitivesCollection.hpp"
//
#include "Simulator/IO/Serialization.hpp"  // all custom, stl and so on

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for the ImposedSystems
     * \ingroup environments
     */
    template <typename PrimitiveList_, typename ContainerPolicy_>
    struct Serialize<::elastica::collision::PrimitivesCollection<
        PrimitiveList_, ContainerPolicy_>> {
      using type =
          ::elastica::collision::PrimitivesCollection<PrimitiveList_,
                                                      ContainerPolicy_>;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& primitives_collection) {
        ar(primitives_collection.data());
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar,
                              type const& primitives_collection) {
        ar(primitives_collection.data());
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica