//==============================================================================
/*!
//  \file
//  \brief H5 support for a blaze subtensor
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

#include <blaze_tensor/math/Subtensor.h>
#include <highfive/H5Group.hpp>
#include <highfive/bits/H5Converter_misc.hpp>
//
#include <algorithm>  // copy
#include <stdexcept>
#include <type_traits>  // trivially_copyable

namespace HighFive {

  namespace details {

    // Subtensor is always row-major, so specialization is not needed for
    // non row-major data structures
    template <typename MT,              // Type of the tensor
              blaze::AlignmentFlag AF,  // Alignment flag
              size_t... CSAs>           // Compile time subtensor arguments
    struct inspector<blaze::Subtensor<MT, AF, CSAs...>> {
      using type = blaze::Subtensor<MT, AF, CSAs...>;
      using value_type = typename type::ElementType;
      using base_type = typename inspector<value_type>::base_type;
      using hdf5_type = base_type;

      static constexpr size_t ndim = 3;
      static constexpr size_t recursive_ndim =
          ndim + inspector<value_type>::recursive_ndim;
      // We force non-trivially copyable since a subtensor may not have
      // contiguous elements which can be copied into a buffer of data
      static constexpr bool is_trivially_copyable = false;

      static std::vector<size_t> getDimensions(const type& val) {
        std::vector<size_t> sizes{static_cast<size_t>(val.pages()),
                                  static_cast<size_t>(val.rows()),
                                  static_cast<size_t>(val.columns())};
        auto s = inspector<value_type>::getDimensions(val(0, 0, 0));
        sizes.insert(sizes.end(), s.begin(), s.end());
        return sizes;
      }

      static size_t getSizeVal(const type& val) {
        return compute_total_size(getDimensions(val));
      }

      static size_t getSize(const std::vector<size_t>& dims) {
        return compute_total_size(dims);
      }

      static void prepare(type& val, const std::vector<size_t>& dims) {
        if (dims[0] != static_cast<size_t>(val.pages()) ||
            dims[1] != static_cast<size_t>(val.rows()) ||
            dims[2] != static_cast<size_t>(val.columns())) {
          throw std::logic_error(
              "Requested size of the subtensor is not the same as the "
              "serialized data! ");
        }

        //   val.resize(static_cast<typename type::Index>(dims[0]),
        //              static_cast<typename type::Index>(dims[1]));
      }

      // These are provided for completion but the H5 backend does not use them
      // Rather, they use the serialize and unserialize functions defined below
      // This is because the type is marked as non-trivially copyable.
      static hdf5_type* data(type& val) {
        throw std::logic_error("Do not call!");
        return inspector<value_type>::data(*(val.data()));
      }

      static const hdf5_type* data(const type& val) {
        throw std::logic_error("Do not call!");
        return inspector<value_type>::data(*(val.data()));
      }

      static void serialize(const type& val, hdf5_type* m) {
        // We know that elements of major type are contiguous, so here we
        // serialize the data slice by slice
        const auto pages = val.pages();
        const auto rows = val.rows();
        const auto cols = val.columns();

        for (std::size_t k = 0UL; k < pages; ++k)
          for (std::size_t i = 0UL; i < rows; ++i)
            std::copy(val.cbegin(i, k), val.cend(i, k),
                      m + (k * rows + i) * cols);
      }

      static void unserialize(const hdf5_type* vec_align,
                              const std::vector<size_t>& dims, type& val) {
        if (dims.size() < 3) {
          std::ostringstream os;
          os << "Impossible to pair DataSet with " << dims.size()
             << " dimensions into a blaze::subtensor.";
          throw DataSpaceException(os.str());
        }

        // prepare gets called here in the backend, so we only bother about the
        // serialization logic
        const auto pages = val.pages();
        const auto rows = val.rows();
        const auto cols = val.columns();

        for (std::size_t k = 0UL; k < pages; ++k)
          for (std::size_t i = 0UL; i < rows; ++i) {
            hdf5_type const* start = vec_align + (k * rows + i) * cols;
            std::copy(start, start + cols, val.data(i, k));
          }
      }
    };

  }  // namespace details

}  // namespace HighFive

namespace elastica {

  namespace io {

    //**************************************************************************
    /*!\brief Write a subtensor to a H5 group
     * \ingroup custom_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    template <typename MT,              // Type of the tensor
              blaze::AlignmentFlag AF,  // Alignment flag
              size_t... CSAs>           // Compile time subtensor arguments
    auto write_data(
        HighFive::Group& group,
        blaze::Subtensor<MT, AF, CSAs...> const& data,  // can be value
        std::string const& data_name) -> HighFive::DataSet {
      return group.createDataSet(data_name, data);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Read a subtensor from a H5 group
     * \ingroup custom_h5
     *
     * \copydetails elastica::io::docs::ReadDataDocStub
     */
    template <typename MT,              // Type of the tensor
              blaze::AlignmentFlag AF,  // Alignment flag
              size_t... CSAs>           // Compile time subtensor arguments
    void read_data(HighFive::Group const& group,
                   blaze::Subtensor<MT, AF, CSAs...>& data,  // can be value
                   std::string const& data_name) {
      auto dataset = group.getDataSet(data_name);
      dataset.read(data);
    }
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
