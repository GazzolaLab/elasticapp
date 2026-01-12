//
// Created by Tejaswin Parthasarathy on 6/14/21.
//

#include <sstream>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/common/Serialize.hpp"
#include "Simulator/Configuration/ModuleConfiguration/common/common.hpp"
#include "Utilities/AsConst.hpp"

namespace {
  struct SSP : public ::elastica::configuration::SingleSizePolicy<SSP> {
    using P = SingleSizePolicy<SSP>;
    using P::P;
  };

  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.SingleSizePolicy",
      "[Unit][Simulator][Configuration]") {
    SSP ssp{5UL};
    CHECK(ssp.value() == 5UL);
    CHECK(std::as_const(ssp).value() == 5UL);
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.SingleSizePolicy.serialize",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using namespace elastica::io;

    {
      std::stringstream ss;  // any stream can be used

      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive
        SSP ssp{5UL};
        // Write the data to the archive
        ::elastica::detail::save_impl(oarchive, ssp);
      }

      // first check the non-throwing variety
      {
        SSP ssp{5UL};
        BinaryInputArchive iarchive(ss);  //  Create an output archive
        ::elastica::detail::load_impl(iarchive, ssp);
      }
    }

    {
      std::stringstream ss;  // any stream can be used
      {
        BinaryOutputArchive oarchive(ss);  //  Create an output archive
        SSP ssp{15UL};
        // Write the data to the archive
        ::elastica::detail::save_impl(oarchive, ssp);
      }

      // first check the non-throwing variety
      {
        SSP ssp{5UL};
        BinaryInputArchive iarchive(ss);  //  Create an output archive

        CHECK_THROWS_WITH(::elastica::detail::load_impl(iarchive, ssp),
                          Catch::Contains("Value from the configuration"));
      }
    }
  }

}  // namespace
