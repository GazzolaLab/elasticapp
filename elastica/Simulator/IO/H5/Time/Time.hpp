//==============================================================================
/*!
//  \file
//  \brief H5 support for types found in \<elastica/Time\>
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

#include "Time/Types.hpp"
//
// workaround for H5_definitions not having std::size_t defined
#include <cstddef>
//
#include <highfive/bits/H5_definitions.hpp>
//
#include <string>

namespace elastica {

  namespace io {

    //**************************************************************************
    /*!\brief Write a ::elastica::Time instance to a H5 group
     * \ingroup time_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    void write_data(HighFive::Group& group, ::elastica::Time const& data,
                    std::string const& data_name);
    //**************************************************************************

    //**************************************************************************
    /*!\brief Write a ::elastica::TimeDelta instance to a H5 group
     * \ingroup time_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    void write_data(HighFive::Group& group, ::elastica::TimeDelta const& data,
                    std::string const& data_name);
    //**************************************************************************

    //**************************************************************************
    /*!\brief Write a ::elastica::TimeStepID instance to a H5 group
     * \ingroup time_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    void write_data(HighFive::Group& group, ::elastica::TimeStepID const& data,
                    std::string const& data_name);
    //**************************************************************************

    //**************************************************************************
    /*!\brief Write a ::elastica::TimeMetadata instance to a H5 group
     * \ingroup time_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    void write_data(HighFive::Group& group,
                    ::elastica::TimeMetadata const& data,
                    std::string const& data_name);
    //**************************************************************************

    //**************************************************************************
    /*!\brief Read a ::elastica::Time instance from a H5 group
     * \ingroup time_h5
     *
     * \copydetails elastica::io::docs::ReadDataDocStub
     */
    void read_data(HighFive::Group const& group, ::elastica::Time& data,
                   std::string const& data_name);
    //**************************************************************************

    //**************************************************************************
    /*!\brief Read a ::elastica::TimeDelta instance from a H5 group
     * \ingroup time_h5
     *
     * \copydetails elastica::io::docs::ReadDataDocStub
     */
    void read_data(HighFive::Group const& group, ::elastica::TimeDelta& data,
                   std::string const& data_name);
    //**************************************************************************

    //**************************************************************************
    /*!\brief Read a ::elastica::TimeStepID instance from a H5 group
     * \ingroup time_h5
     *
     * \copydetails elastica::io::docs::ReadDataDocStub
     */
    void read_data(HighFive::Group const& group, ::elastica::TimeStepID& data,
                   std::string const& data_name);
    //**************************************************************************

    //**************************************************************************
    /*!\brief Read a ::elastica::TimeMetadata instance from a H5 group
     * \ingroup time_h5
     *
     * \copydetails elastica::io::docs::ReadDataDocStub
     */
    void read_data(HighFive::Group const& group, ::elastica::TimeMetadata& data,
                   std::string const& data_name);
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
