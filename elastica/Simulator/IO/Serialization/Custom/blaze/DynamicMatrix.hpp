//==============================================================================
/*!
//  \file
//  \brief Serialization support for a blaze DynamicMatrix
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

#include <blaze/math/DynamicMatrix.h>
#include <exception>

#include "ErrorHandling/Assert.hpp"
#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for blaze::DynamicMatrix
    // \ingroup custom_serialize
    */
    template <typename Type,   // Data type of the matrix
              bool SO,         // Storage order
              typename Alloc,  // Type of the allocator
              typename Tag>    // Type tag
    struct Serialize<blaze::DynamicMatrix<Type, SO, Alloc, Tag>> {
      using type = blaze::DynamicMatrix<Type, SO, Alloc, Tag>;
      static_assert(std::is_arithmetic<Type>::value,
                    "Not possible to serialize blaze types for non arithmetic "
                    "value types!");

      template <typename InArchive>
      static inline void load(InArchive& ar, type& matrix) {
        std::size_t rows, cols, spacing, capacity;
        ar(make_size_tag(rows));
        ar(make_size_tag(cols));
        ar(make_size_tag(spacing));
        ar(make_size_tag(capacity));

        matrix.resize(rows, cols, false);
        if (matrix.capacity() != capacity) {
          throw std::runtime_error(
              "matrix's resized capacity is not the same as stored capacity");
        }
        ar(binary_data(matrix.data(),
                       static_cast<std::size_t>(capacity) * sizeof(Type)));
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& matrix) {
        ar(make_size_tag(static_cast<std::size_t>(matrix.rows())));
        ar(make_size_tag(static_cast<std::size_t>(matrix.columns())));
        ar(make_size_tag(static_cast<std::size_t>(matrix.spacing())));
        ar(make_size_tag(static_cast<std::size_t>(matrix.capacity())));
        ar(binary_data(
            matrix.data(),
            static_cast<std::size_t>(matrix.capacity()) * sizeof(Type)));
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica