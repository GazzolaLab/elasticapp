#pragma once

#include <iostream>
#include <string>

#include "Simulator/Configuration/ModuleConfiguration/ExecutionPolicy.hpp"

namespace elastica {

  namespace configuration {

    // TODO : move into CPP files

    //**************************************************************************
    /*!\name stringify execution policies*/
    /*!\ingroup execution*/
    //@{
    inline std::string to_string(sequential_policy const&) {
      return "sequential policy";
    }
    inline std::string to_string(parallel_policy const&) {
      return "parallel policy";
    }
    inline std::string to_string(hybrid_policy const&) {
      return "hybrid sequential-parallel policy";
    }
    //@}
    //**************************************************************************

    //**************************************************************************
    /*!\name stream execution policies*/
    /*!\ingroup execution*/
    //@{
    inline std::ostream& operator<<(std::ostream& os,
                                    sequential_policy const& p) {
      using ::elastica::configuration::to_string;
      return os << to_string(p);
    }
    inline std::ostream& operator<<(std::ostream& os,
                                    parallel_policy const& p) {
      using ::elastica::configuration::to_string;
      return os << to_string(p);
    }
    inline std::ostream& operator<<(std::ostream& os, hybrid_policy const& p) {
      using ::elastica::configuration::to_string;
      return os << to_string(p);
    }
    //@}
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica