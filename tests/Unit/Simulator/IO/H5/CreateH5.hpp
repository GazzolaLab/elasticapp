//
// Created by Tejaswin Parthasarathy on 7/23/22.
//

#pragma once

#include <cstdio>
#include <highfive/H5File.hpp>
#include <string>
#include <utility>

//! suffix is intended to be thread specific, something like thread id
inline auto generate_h5(std::string suffix = "0") -> HighFive::Group {
  // Have the same name so that a bunch of files are not created
  const std::string file_name("test_dataset_" + std::move(suffix) + ".h5");
  // remove everytime to prevent HDF from throwing up
  std::remove(file_name.c_str());
  HighFive::File file(file_name, HighFive::File::ReadWrite |
                                     HighFive::File::Create |
                                     HighFive::File::Truncate);
  return file.createGroup("main");
}