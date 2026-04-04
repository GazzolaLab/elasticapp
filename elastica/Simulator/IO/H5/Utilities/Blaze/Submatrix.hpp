//==============================================================================
/*!
//  \file
//  \brief H5 support for a blaze submatrix
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

#include <blaze/math/Submatrix.h>
#include <highfive/H5Group.hpp>
#include <highfive/bits/H5Converter_misc.hpp>
//
#include <algorithm>  // copy
#include <stdexcept>
#include <type_traits>  // trivially_copyable

namespace HighFive {

  namespace details {

    template <typename MT,              // Type of the matrix
              blaze::AlignmentFlag AF,  // Alignment flag
              bool SO,                  // Storage Order
              bool DF,                  // Density flag
              size_t... CSAs>           // Compile time submatrix arguments
    struct inspector<blaze::Submatrix<MT, AF, SO, DF, CSAs...>> {
      using type = blaze::Submatrix<MT, AF, SO, DF, CSAs...>;
      using value_type = typename type::ElementType;
      using base_type = typename inspector<value_type>::base_type;
      using hdf5_type = base_type;

      static constexpr size_t ndim = 2;
      static constexpr size_t min_ndim = ndim + inspector<value_type>::min_ndim;
      static constexpr size_t max_ndim = ndim + inspector<value_type>::max_ndim;
      // We force non-trivially copyable since a submatrix may not have
      // contiguous elements which can be copied into a buffer of data
      static constexpr bool is_trivially_copyable = false;
      static constexpr bool column_major = blaze::IsColumnMajorMatrix<MT>::value;

      template <typename T>
      static auto get_stride(T&& val) -> std::size_t {
        return column_major ? val.rows() : val.columns();
      }

      template <typename T>
      static auto get_size(T&& val) -> std::size_t {
        return column_major ? val.columns() : val.rows();
      }

      static size_t getRank(const type& val) {
        if (get_size(val))
          return ndim + inspector<value_type>::getRank(val(0, 0));
        else
          return min_ndim;
      }

      static std::vector<size_t> getDimensions(const type& val) {
        std::vector<size_t> sizes{static_cast<size_t>(get_size(val)),
                                  static_cast<size_t>(get_stride(val))};
        auto s = inspector<value_type>::getDimensions(val(0, 0));
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
        if (dims[0] != static_cast<size_t>(get_size(val)) ||
            dims[1] != static_cast<size_t>(get_stride(val))) {
          throw std::logic_error(
              "Requested size of the submatrix is not the same as the "
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

      static void serialize(const type& val, const std::vector<size_t>& /* dims*/, hdf5_type* m) {
        // We know that elements of major type are contiguous, so here we
        // serialize the data row by row.
        const auto stride = get_stride(val);
        const auto trips = get_size(val);
        for (std::size_t trip = 0UL; trip < trips; ++trip)
          std::copy(val.cbegin(trip), val.cend(trip), m + trip * stride);
      }

      static void unserialize(const hdf5_type* vec_align,
                              const std::vector<size_t>& dims, type& val) {
        if (dims.size() < 2) {
          std::ostringstream os;
          os << "Impossible to pair DataSet with " << dims.size()
             << " dimensions into a blaze::submatrix.";
          throw DataSpaceException(os.str());
        }

        // prepare gets called here in the backend, so we only bother about the
        // serialization logic
        const auto stride = get_stride(val);
        const auto trips = get_size(val);
        for (std::size_t trip = 0UL; trip < trips; ++trip)
          std::copy(vec_align + trip * stride, vec_align + (trip + 1) * stride,
                    val.data(trip));
      }
    };

    /*
    // https://stackoverflow.com/a/22414046
    template <typename MT,              // Type of the matrix
              blaze::AlignmentFlag AF,  // Alignment flag
              bool SO,                  // Storage Order
              bool DF,                  // Density flag
              size_t... CSAs>           // Compile time submatrix arguments
    constexpr bool inspector<blaze::Submatrix<MT, AF, SO, DF, CSAs...>>::column_major;
    */

  }  // namespace details

}  // namespace HighFive

namespace elastica {

  namespace io {

    //**************************************************************************
    /*!\brief Write a submatrix to a H5 group
     * \ingroup custom_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    template <typename MT,              // Type of the matrix
              blaze::AlignmentFlag AF,  // Alignment flag
              bool SO,                  // Storage Order
              bool DF,                  // Density flag
              size_t... CSAs>           // Compile time submatrix arguments
    auto write_data(
        HighFive::Group& group,
        blaze::Submatrix<MT, AF, SO, DF, CSAs...> const& data,  // can be value
        std::string const& data_name) -> HighFive::DataSet {
      return group.createDataSet(data_name, data);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Read a submatrix from a H5 group
     * \ingroup custom_h5
     *
     * \copydetails elastica::io::docs::ReadDataDocStub
     */
    template <typename MT,              // Type of the matrix
              blaze::AlignmentFlag AF,  // Alignment flag
              bool SO,                  // Storage Order
              bool DF,                  // Density flag
              size_t... CSAs>           // Compile time submatrix arguments
    void read_data(
        HighFive::Group const& group,
        blaze::Submatrix<MT, AF, SO, DF, CSAs...>& data,  // can be value
        std::string const& data_name) {
      auto dataset = group.getDataSet(data_name);
      dataset.read(data);
    }
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
