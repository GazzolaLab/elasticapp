//==============================================================================
/*!
//  \file
//  \brief Serialization support for a blaze DynamicTensor
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

#include <blaze_tensor/math/DynamicTensor.h>
#include <exception>

#include "ErrorHandling/Assert.hpp"
#include "Simulator/IO/Serialization/Helpers.hpp"
#include "Simulator/IO/Serialization/Serialize.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    /*!\brief Specialization of Serialize template for blaze::DynamicTensor
    // \ingroup custom_serialize
    */
    template <typename Type>  // Data type of the tensor
    struct Serialize<blaze::DynamicTensor<Type>> {
      using type = blaze::DynamicTensor<Type>;
      static_assert(std::is_arithmetic<Type>::value,
                    "Not possible to serialize blaze types for non arithmetic "
                    "value types!");

      template <typename InArchive>
      static inline void load(InArchive& ar, type& tensor) {
        std::size_t pages, rows, cols, spacing, capacity;
        ar(make_size_tag(pages));
        ar(make_size_tag(rows));
        ar(make_size_tag(cols));
        ar(make_size_tag(spacing));
        ar(make_size_tag(capacity));

        tensor.resize(pages, rows, cols, false);
        if (tensor.capacity() != capacity) {
          throw std::runtime_error(
              "tensor's resized capacity is not the same as stored capacity");
        }
        ar(binary_data(tensor.data(),
                       static_cast<std::size_t>(capacity) * sizeof(Type)));
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& tensor) {
        ar(make_size_tag(static_cast<std::size_t>(tensor.pages())));
        ar(make_size_tag(static_cast<std::size_t>(tensor.rows())));
        ar(make_size_tag(static_cast<std::size_t>(tensor.columns())));
        ar(make_size_tag(static_cast<std::size_t>(tensor.spacing())));
        ar(make_size_tag(static_cast<std::size_t>(tensor.capacity())));
        ar(binary_data(
            tensor.data(),
            static_cast<std::size_t>(tensor.capacity()) * sizeof(Type)));
      }
    };
    /*! \endcond */
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
