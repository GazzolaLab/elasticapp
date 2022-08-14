//
// Created by Tejaswin Parthasarathy on 7/23/22.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/IO/H5/CreateH5.hpp"
#include "Simulator/IO/H5/STL/vector.hpp"  // needed for serializing vectors in H5
#include "Simulator/IO/H5/Utilities/TaggedTuple.hpp"
#include "Utilities/CountingIterator.hpp"
#include "Utilities/StdHelpers.hpp"
//
#include <vector>

namespace {

  struct V1Tag {
    using type = std::vector<int>;
  };
  struct V2Tag {
    using type = std::vector<float>;
  };

  ELASTICA_TEST_CASE("Unit.Simulator.IO.H5.TaggedTuple",
                     "[Unit][Simulator][IO]") {
    auto group = generate_h5();
    // Setup
    using T = tuples::TaggedTuple<V1Tag, V2Tag>;
    std::vector<int> iv{1, 2, 3, 4, 5};
    std::vector<float> fv{1.1, 2.2, 3.3, 4.4, 5.5};
    T tup(std::move(iv), std::move(fv));

    // Write and flush
    elastica::io::write_data(group, tup, "data");
    group.getFile().flush();

    // Clobber data
    T another_tup{};
    tuples::get<V1Tag>(another_tup).resize(5UL);
    tuples::get<V2Tag>(another_tup).resize(5UL);

    // Read data back
    elastica::io::read_data(group, another_tup, "data");

    // Post check
    CHECK(tup == another_tup);
  }

}  // namespace
