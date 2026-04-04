//==============================================================================
/*!
//  \file
//  \brief H5 support for \elastica rigid bodies
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

//
#include "Systems/Block/Block/VariableCache.hpp"
#include "Systems/RigidBody/BlockSlice.hpp"
#include "Systems/RigidBody/RigidBodyPlugin.hpp"
//
#include "Simulator/IO/H5/Systems/BlockSlice.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*!\brief Write a RigidBody slice to a H5 group
     * \ingroup systems_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    template <typename RBT,
              template <typename /*RBT*/, typename /*InitializedBlock*/>
              class... Components>
    auto write_data(HighFive::Group& group,
                    ::blocks::BlockSlice<elastica::rigid_body::RigidBodyPlugin<
                        RBT, ::blocks::BlockSlice, Components...>> const& data,
                    std::string const& data_name) {
      return ::blocks::write_slice_to_h5(group, data, data_name);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Write a ConstRigidBody slice to a H5 group
     * \ingroup systems_h5
     *
     * \copydetails elastica::io::docs::ReadDataDocStub
     */
    template <typename RBT,
              template <typename /*RBT*/, typename /*InitializedBlock*/>
              class... Components>
    void write_data(
        HighFive::Group& group,
        ::blocks::ConstBlockSlice<elastica::rigid_body::RigidBodyPlugin<
            RBT, ::blocks::ConstBlockSlice, Components...>> const& data,
        std::string const& data_name) {
      ::blocks::write_slice_to_h5(group, data, data_name);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Write a RigidBody variable cache to a H5 group
     * \ingroup systems_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    template <typename RBT,
              template <typename /*RBT*/, typename /*InitializedBlock*/>
              class... Components,
              typename... Tags>
    auto write_data(
        HighFive::Group& group,
        ::blocks::VariableCache<
            ::blocks::BlockSlice<elastica::rigid_body::RigidBodyPlugin<
                RBT, ::blocks::BlockSlice, Components...>>,
            Tags...> const& data,
        std::string const& data_name) {
      return ::blocks::write_slice_to_h5(group, data, data_name);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Read a RigidBody slice from a H5 group
     * \ingroup systems_h5
     */
    template <typename RBT,
              template <typename /*RBT*/, typename /*InitializedBlock*/>
              class... Components>
    void read_data(HighFive::Group const& group,
                   ::blocks::BlockSlice<elastica::rigid_body::RigidBodyPlugin<
                       RBT, ::blocks::BlockSlice, Components...>>& data,
                   std::string const& data_name) {
      ::blocks::read_slice_from_h5(group, data, data_name);
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Write a RigidBody variable cache to a H5 group
     * \ingroup systems_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    template <typename RBT,
              template <typename /*RBT*/, typename /*InitializedBlock*/>
              class... Components,
              typename... Tags>
    auto read_data(
        HighFive::Group& group,
        ::blocks::VariableCache<
            ::blocks::BlockSlice<elastica::rigid_body::RigidBodyPlugin<
                RBT, ::blocks::BlockSlice, Components...>>,
            Tags...>& data,
        std::string const& data_name) {
      return ::blocks::read_slice_from_h5(group, data, data_name);
    }
    //**************************************************************************

  }  // namespace io

}  // namespace elastica