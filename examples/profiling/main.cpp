//==============================================================================
/*!
//  \file
//  \brief Time profile example
//
//  See LICENSE for details.
*/
//==============================================================================

#include "Simulator/Connections.hpp"
#include "Simulator/Constraints.hpp"
#include "Simulator/Environments.hpp"
#include "Simulator/Forcing.hpp"
#include "Simulator/Simulator.hpp"
//
// CosseratRod is a default system now, so we include it
#include "Systems/CosseratRods.hpp"
//
#include "Simulator/IO/Serialization.hpp"
#include "Simulator/IO/Serialization/Archives.hpp"  // all archives
//
#include <chrono>
#include <thread>

#include "Simulator/Utilities/Serialize.hpp"
#include "Simulator/Utilities/Timers.hpp"

void sleep_for_2ms() {
  std::this_thread::sleep_for(std::chrono::milliseconds(2));
}

int main() {
  /*
   * Basic use of a wall-clock timer.
   * For more detailed examples, consult the documentation of
   * elastica::timing::Timer.
   */
  {
    // Creating a new wall clock timer
    elastica::timing::WcTimer timer;

    // Starting the time measurement
    timer.start();

    sleep_for_2ms();  // Program or code fragment to be measured

    // Stopping the time measurement
    timer.stop();
  }

  /*
   * Use of timers in a simulator.
   *
   * Timers are a tagged collection of elastica::timing::Timer and represent
   * various phases/sections of execution of our computational graph. The
   * simulator comes loaded with these timers, and can be located with
   * simulator->timers() command. Consult its documentation for more details.
   */
  // Construct a dummy simulator to get the timer we  want
  auto simulator = elastica::make_simulator<
      elastica::modules::Connections, elastica::modules::Constraints,
      elastica::modules::Environments, elastica::modules::Forcing>();
  simulator->finalize();

  // Influence profiling information artificially by sleeping
  for (std::size_t trips = 0UL; trips < 5UL; ++trips)
    elastica::for_each(simulator->timers(), [](auto& timer) {
      timer.start();
      sleep_for_2ms();
      timer.stop();
    });

  // Streaming the timer prints out a csv file that is numpy compatible
  std::cout << "Profiling information" << std::endl;
  std::cout << simulator->timers() << std::endl;

  std::ofstream outcsv("filename.csv", std::ios_base::out);
  outcsv << simulator->timers();

  // One can also serialize it using elastica::Archives
  std::ofstream out("filename.yml", std::ios_base::out);
  {
    using namespace elastica::io;
    using Archive =
#if defined(ELASTICA_USE_YAMLCPP)
        YAMLOutputArchive;
#else
        BinaryOutputArchive;
#endif
    Archive oarchive{out};  //  Create an output archive
    // Write the data to it
    oarchive(make_nvp("timers", simulator->timers()));
  }
}
