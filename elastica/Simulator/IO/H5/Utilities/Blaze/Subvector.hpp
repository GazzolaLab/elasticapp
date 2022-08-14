//==============================================================================
/*!
//  \file
//  \brief H5 support for a blaze subvector
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

#include <blaze/math/Subvector.h>
#include <highfive/H5Group.hpp>
#include <highfive/bits/H5Converter_misc.hpp>
//
#include <algorithm>    // copy
#include <type_traits>  // trivially_copyable

namespace HighFive {

  namespace details {

    template <typename VT,              // Type of the vector
              blaze::AlignmentFlag AF,  // Alignment flag
              bool TF,                  // Transpose flag
              bool DF,                  // Density flag
              size_t... CSAs>           // Compile time subvector arguments
    struct inspector<blaze::Subvector<VT, AF, TF, DF, CSAs...>> {
      using type = blaze::Subvector<VT, AF, TF, DF, CSAs...>;
      using value_type = typename type::ElementType;
      using base_type = typename inspector<value_type>::base_type;
      using hdf5_type = base_type;

      static constexpr size_t ndim = 1;
      static constexpr size_t recursive_ndim =
          ndim + inspector<value_type>::recursive_ndim;
      static constexpr bool is_trivially_copyable =
          std::is_trivially_copyable<value_type>::value;

      static std::vector<size_t> getDimensions(const type& val) {
        std::vector<size_t> sizes{static_cast<size_t>(val.size())};
        if (val.size()) {
          auto s = inspector<value_type>::getDimensions(val[0]);
          sizes.insert(sizes.end(), s.begin(), s.end());
        }
        return sizes;
      }

      static size_t getSizeVal(const type& val) {
        return compute_total_size(getDimensions(val));
      }

      static size_t getSize(const std::vector<size_t>& dims) {
        return compute_total_size(dims);
      }

      static void prepare(type& val, const std::vector<size_t>& dims) {
        // Assuming only POD types here, so resize() call is enough
        if (dims[0] != static_cast<size_t>(val.size())) {
          throw std::logic_error(
              "Requested size of the subvector is not the same as the "
              "serialized HDF5 data array!");
        }
        // val.resize(static_cast<typename type::Index>(dims[0]));
      }

      static hdf5_type* data(type& val) {
        return inspector<value_type>::data(val[0]);
      }

      static const hdf5_type* data(const type& val) {
        return inspector<value_type>::data(val[0]);
      }

      static void serialize(const type& val, hdf5_type* m) {
        std::copy(val.cbegin(), val.cend(), m);
      }

      static void unserialize(const hdf5_type* vec_align,
                              const std::vector<size_t>& dims, type& val) {
        if (dims.size() != 1) {
          std::ostringstream os;
          os << "Impossible to pair DataSet with " << dims.size()
             << " dimensions into a blaze::subvector.";
          throw DataSpaceException(os.str());
        }
        std::copy(vec_align, vec_align + dims[0], val.data());
      }
    };

  }  // namespace details

}  // namespace HighFive

namespace elastica {

  namespace io {

    //**************************************************************************
    /*!\brief Write a subvector to a H5 group
     * \ingroup custom_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    template <typename VT,              // Type of the vector
              blaze::AlignmentFlag AF,  // Alignment flag
              bool TF,                  // Transpose flag
              bool DF,                  // Density flag
              size_t... CSAs>           // Compile time subvector arguments
    auto write_data(
        HighFive::Group& group,
        blaze::Subvector<VT, AF, TF, DF, CSAs...> const& data,  // can be value
        std::string const& data_name) -> HighFive::DataSet {
      return group.createDataSet(data_name, data);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Read a subvector from a H5 group
     * \ingroup custom_h5
     *
     * \copydetails elastica::io::docs::ReadDataDocStub
     */
    template <typename VT,              // Type of the vector
              blaze::AlignmentFlag AF,  // Alignment flag
              bool TF,                  // Transpose flag
              bool DF,                  // Density flag
              size_t... CSAs>           // Compile time subvector arguments
    void read_data(
        HighFive::Group const& group,
        blaze::Subvector<VT, AF, TF, DF, CSAs...>& data,  // can be value
        std::string const& data_name) {
      auto dataset = group.getDataSet(data_name);
      dataset.read(data);
    }
    //**************************************************************************

  }  // namespace io

}  // namespace elastica