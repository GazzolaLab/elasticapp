//==============================================================================
/*!
//  \file
//  \brief H5 support for types found in \<TaggedTuple.h\>
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

#include <exception>
#include <highfive/H5Group.hpp>

#include "Utilities/PrettyType.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"

namespace elastica {

  namespace io {

    //**************************************************************************
    /*!\brief Write a tagged tuple to a H5 group
     * \ingroup custom_h5
     *
     * \copydetails elastica::io::docs::WriteDataDocStub
     */
    template <class... Tags>
    void write_data(HighFive::Group& group,
                    tuples::TaggedTuple<Tags...> const& data,
                    std::string const& data_name) {
      auto new_group = group.createGroup(group.getPath() + "/" + data_name);
      tmpl::for_each<tmpl::list<Tags...>>([&](auto v) {
        using Tag = tmpl::type_from<decltype(v)>;
        auto name{pretty_type::name<Tag>()};
        try {
          auto next_group =
              new_group.createGroup(new_group.getPath() + "/" + name);
          write_data(next_group, tuples::get<Tag>(data), "data");
        } catch (const std::runtime_error& e) {
          std::throw_with_nested(std::runtime_error(
              "Error while writing H5 data tagged with " + name));
        }
      });
    }
    //**************************************************************************

    //**************************************************************************
    /*!\brief Read a tagged tuple from a H5 group
     * \ingroup custom_h5
     *
     * \copydetails elastica::io::docs::ReadDataDocStub
     */
    template <class... Tags>
    void read_data(HighFive::Group const& group,
                   tuples::TaggedTuple<Tags...>& data,
                   std::string const& data_name) {
      auto const new_group = group.getGroup(group.getPath() + "/" + data_name);
      tmpl::for_each<tmpl::list<Tags...>>([&](auto v) {
        using Tag = tmpl::type_from<decltype(v)>;
        auto name{pretty_type::name<Tag>()};
        try {
          auto const next_group =
              new_group.getGroup(new_group.getPath() + "/" + name);
          read_data(next_group, tuples::get<Tag>(data), "data");
        } catch (const std::runtime_error& e) {
          std::throw_with_nested(std::runtime_error(
              "Error while reading H5 data tagged with " + name));
        }
      });
    }
    //**************************************************************************

  }  // namespace io

}  // namespace elastica
