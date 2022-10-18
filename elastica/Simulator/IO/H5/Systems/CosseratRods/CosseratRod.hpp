//==============================================================================
/*!
//  \file
//  \brief H5 support for \elastica CosseratRod
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

//
#include "Systems/Block/Block/VariableCache.hpp"
#include "Systems/CosseratRods/BlockSlice.hpp"
#include "Systems/CosseratRods/CosseratRodPlugin.hpp"
//
#include "Simulator/IO/H5/Systems/BlockSlice.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*!\brief Write a CosseratRod slice to a H5 group
     * \ingroup systems_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    template <typename CRT,
              template <typename /*CRT*/, typename /*InitializedBlock*/>
              class... Components>
    auto write_data(
        HighFive::Group& group,
        ::blocks::BlockSlice<elastica::cosserat_rod::CosseratRodPlugin<
            CRT, ::blocks::BlockSlice, Components...>> const& data,
        std::string const& data_name) {
      return ::blocks::write_slice_to_h5(group, data, data_name);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Write a ConstCosseratRod slice to a H5 group
     * \ingroup systems_h5
     *
     * \copydetails elastica::io::docs::ReadDataDocStub
     */
    template <typename CRT,
              template <typename /*CRT*/, typename /*InitializedBlock*/>
              class... Components>
    void write_data(
        HighFive::Group& group,
        ::blocks::ConstBlockSlice<elastica::cosserat_rod::CosseratRodPlugin<
            CRT, ::blocks::ConstBlockSlice, Components...>> const& data,
        std::string const& data_name) {
      ::blocks::write_slice_to_h5(group, data, data_name);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Write a CosseratRod variable cache to a H5 group
     * \ingroup systems_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    template <typename CRT,
              template <typename /*CRT*/, typename /*InitializedBlock*/>
              class... Components,
              typename... Tags>
    auto write_data(
        HighFive::Group& group,
        ::blocks::VariableCache<
            ::blocks::BlockSlice<elastica::cosserat_rod::CosseratRodPlugin<
                CRT, ::blocks::BlockSlice, Components...>>,
            Tags...> const& data,
        std::string const& data_name) {
      return ::blocks::write_slice_to_h5(group, data, data_name);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Read a CosseratRod slice from a H5 group
     * \ingroup systems_h5
     */
    template <typename CRT,
              template <typename /*CRT*/, typename /*InitializedBlock*/>
              class... Components>
    void read_data(
        HighFive::Group const& group,
        ::blocks::BlockSlice<elastica::cosserat_rod::CosseratRodPlugin<
            CRT, ::blocks::BlockSlice, Components...>>& data,
        std::string const& data_name) {
      ::blocks::read_slice_from_h5(group, data, data_name);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Write a CosseratRod slice to a H5 group
     * \ingroup systems_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    template <typename CRT,
              template <typename /*CRT*/, typename /*InitializedBlock*/>
              class... Components,
              typename... Tags>
    auto read_data(
        HighFive::Group& group,
        ::blocks::VariableCache<
            ::blocks::BlockSlice<elastica::cosserat_rod::CosseratRodPlugin<
                CRT, ::blocks::BlockSlice, Components...>>,
            Tags...>& data,
        std::string const& data_name) {
      return ::blocks::read_slice_from_h5(group, data, data_name);
    }
    //**************************************************************************

  }  // namespace io

}  // namespace elastica