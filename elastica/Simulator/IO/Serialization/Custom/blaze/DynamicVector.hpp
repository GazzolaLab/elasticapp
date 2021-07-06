//==============================================================================
/*!
//  \file
//  \brief Serialization support for a blaze DynamicVector
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

#include <blaze/math/DynamicVector.h>
#include <exception>

#include "ErrorHandling/Assert.hpp"
#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for blaze::DynamicVector
    // \ingroup custom_serialize
    */
    template <typename Type,   // Data type of the matrix
              bool TF,         // Transpose flag
              typename Alloc,  // Type of the allocator
              typename Tag>    // Type tag
    struct Serialize<blaze::DynamicVector<Type, TF, Alloc, Tag>> {
      using type = blaze::DynamicVector<Type, TF, Alloc, Tag>;
      static_assert(std::is_arithmetic<Type>::value,
                    "Not possible to serialize blaze types for non arithmetic "
                    "value types!");

      template <typename InArchive>
      static inline void load(InArchive& ar, type& vector) {
        std::size_t size, spacing, capacity;
        ar(make_size_tag(size));
        ar(make_size_tag(spacing));
        ar(make_size_tag(capacity));

        vector.resize(size, false);
        if (vector.capacity() != capacity) {
          throw std::runtime_error(
              "vectors's resized capacity is not the same as stored capacity");
        }
        ar(binary_data(vector.data(),
                       static_cast<std::size_t>(capacity) * sizeof(Type)));
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& vector) {
        ar(make_size_tag(static_cast<std::size_t>(vector.size())));
        ar(make_size_tag(static_cast<std::size_t>(vector.spacing())));
        ar(make_size_tag(static_cast<std::size_t>(vector.capacity())));
        ar(binary_data(
            vector.data(),
            static_cast<std::size_t>(vector.capacity()) * sizeof(Type)));
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
