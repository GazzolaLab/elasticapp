//==============================================================================
/*!
//  \file
//  \brief H5 support for types found in \<vector\>
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

#include <highfive/H5DataSet.hpp>
#include <highfive/H5Group.hpp>
#include <vector>

namespace elastica {

  namespace io {

    //**************************************************************************
    /*!\brief Write a std::vector to a H5 group
     * \ingroup stl_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    template <typename T, class Allocator>
    auto write_data(HighFive::Group& group,
                    std::vector<T, Allocator> const& data,
                    std::string const& data_name) -> HighFive::DataSet {
      return group.createDataSet(data_name, data);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Write a std::vector to a H5 group
     * \ingroup stl_h5
     *
     * \copydetails elastica::io::docs::ReadDataDocStub
     */
    template <typename T, class Allocator>
    void read_data(HighFive::Group const& group,
                   std::vector<T, Allocator>& data,
                   std::string const& data_name) {
      auto dataset = group.getDataSet(data_name);
      dataset.read(data);
    }
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
