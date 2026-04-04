//==============================================================================
/*!
//  \file
//  \brief H5 support for types found in \<elastica/Time\>
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
*/
//==============================================================================

#include "Simulator/IO/H5/Time/Time.hpp"
//
#include "Time/Time.hpp"
//
#include <highfive/H5DataSet.hpp>
#include <highfive/H5File.hpp>
#include <highfive/H5Group.hpp>

namespace elastica {

  namespace io {

    namespace {

      template <typename T>
      auto write_data_impl(HighFive::Group& group, T const& data,
                           std::string const& data_name) -> HighFive::DataSet {
        return group.createDataSet(data_name, data.value());
      }

      template <typename T>
      void read_data_impl(HighFive::Group const& group, T& data,
                          std::string const& data_name) {
        auto dataset = group.getDataSet(data_name);
        dataset.read(data.value());
      }

    }  // namespace

    void write_data(HighFive::Group& group, ::elastica::Time const& data,
                    std::string const& data_name) {
      write_data_impl(group, data, data_name);
    }

    void write_data(HighFive::Group& group, ::elastica::TimeDelta const& data,
                    std::string const& data_name) {
      write_data_impl(group, data, data_name);
    }

    void write_data(HighFive::Group& group, ::elastica::TimeStepID const& data,
                    std::string const& data_name) {
      auto metadata_group =
          group.createGroup(group.getPath() + "/" + data_name);
      metadata_group.createDataSet("threshold", data.step_threshold());
      metadata_group.createDataSet("step", data.step());
      metadata_group.createDataSet("substep", data.substep());
    }

    void write_data(HighFive::Group& group,
                    ::elastica::TimeMetadata const& data,
                    std::string const& data_name) {
      auto metadata_group =
          group.createGroup(group.getPath() + "/" + data_name);
      write_data(metadata_group, data.time_v, "time");
      write_data(metadata_group, data.dt, "dt");
      write_data(metadata_group, data.id, "Timestep");
    }

    void read_data(HighFive::Group const& group, ::elastica::Time& data,
                   std::string const& data_name) {
      read_data_impl(group, data, data_name);
    }

    void read_data(HighFive::Group const& group, ::elastica::TimeDelta& data,
                   std::string const& data_name) {
      read_data_impl(group, data, data_name);
    }

    void read_data(HighFive::Group const& group, ::elastica::TimeStepID& data,
                   std::string const& data_name) {
      auto const metadata_group =
          group.getGroup(group.getPath() + "/" + data_name);
      {
        auto dataset = metadata_group.getDataSet("threshold");
        dataset.read(data.step_threshold());
      }
      {
        auto dataset = metadata_group.getDataSet("step");
        dataset.read(data.step());
      }
      {
        auto dataset = metadata_group.getDataSet("substep");
        dataset.read(data.substep());
      }
    }

    void read_data(HighFive::Group const& group, ::elastica::TimeMetadata& data,
                   std::string const& data_name) {
      auto const metadata_group =
          group.getGroup(group.getPath() + "/" + data_name);
      read_data(metadata_group, data.time_v, "time");
      read_data(metadata_group, data.dt, "dt");
      read_data(metadata_group, data.id, "Timestep");
    }

  }  // namespace io

}  // namespace elastica