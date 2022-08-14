//==============================================================================
/*!
//  \file
//  \brief H5 support for Block slices
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
#include "Simulator/IO/H5/Utilities.hpp"
//
#include <highfive/H5DataSet.hpp>
#include <highfive/H5Group.hpp>
#include <string>
#include <utility>

namespace blocks {

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Write block slices to H5
  // \ingroup systems_h5
  */
  template <typename Slice>
  void write_slice_to_h5(HighFive::Group& group, Slice&& slice,
                         std::string const& data_name) {
    return ::elastica::io::write_data(group, std::forward<Slice>(slice).data(),
                                      data_name);
  }
  /*! \endcond */
  //****************************************************************************

  //****************************************************************************
  /*! \cond ELASTICA_INTERNAL */
  /*!\brief Read block slices from H5
  // \ingroup systems_h5
  */
  template <typename Slice>
  void read_slice_from_h5(HighFive::Group const& group, Slice&& slice,
                          std::string const& data_name) {
    ::elastica::io::read_data(group, std::forward<Slice>(slice).data(),
                              data_name);
  }
  /*! \endcond */
  //****************************************************************************

}  // namespace blocks
