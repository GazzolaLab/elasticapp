
#include "Framework/TestingFramework.hpp"
#include "Simulator/IO/Serialization/Archives.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"

// all test routines
#include "detail/Test_Array.hpp"
#include "detail/Test_BlazeDynamicMatrix.hpp"
#include "detail/Test_BlazeDynamicTensor.hpp"
#include "detail/Test_BlazeDynamicVector.hpp"
#include "detail/Test_CustomStructs.hpp"
#include "detail/Test_POD.hpp"
#include "detail/Test_String.hpp"
#include "detail/Test_TaggedTuple.hpp"
#include "detail/Test_Tuple.hpp"
#include "detail/Test_UnorderedMap.hpp"
#include "detail/Test_Vector.hpp"

/// [custom_serialize]
struct MyImportantInfo {
  int i;
  double d;
};

namespace elastica {

  namespace io {

    template <>
    struct Serialize<MyImportantInfo> {
      using type = MyImportantInfo;

      template <typename InArchive>
      static inline void load(InArchive& ar, type& info) {
        ar(info.i, info.d);
      }

      template <typename OutArchive>
      static inline void save(OutArchive& ar, type const& info) {
        ar(info.i, info.d);
      }
    };

  }  // namespace io

}  // namespace elastica
/// [custom_serialize]

void test_custom_serialize() {
  /// [custom_serialize_usage]
  using namespace elastica::io;
  std::stringstream ss;  // any stream can be used
  MyImportantInfo o_info{2, 2.0};
  BinaryOutputArchive oarchive(ss);  //  Create an output archive
  oarchive(o_info);                  // Write the data to the archive

  // same usage with input archives for loading

  /// [custom_serialize_usage]
}

namespace {

  // These are overall tests
  // it is hard to unit-test each function because most of them are just C++
  // sugar without too much functionality
  using namespace elastica::io;

  void IGNORE_UNUSED test_binary_archive() {
    /// [binary_archive]
    std::stringstream ss;  // any stream can be used
    int o_i{2};
    float o_f{2.0F};
    double o_d{2.0};
    std::string o_s{"2.0F"};
    {
      BinaryOutputArchive oarchive(ss);  //  Create an output archive

      oarchive(o_i, o_f, o_d, o_s);  // Write the data to the archive
    }  // archive goes out of scope, ensuring all contents are flushed

    // uninitialized, but thats okay
    int i_i;
    float i_f;
    double i_d;
    std::string i_s;
    {
      BinaryInputArchive iarchive(ss);  //  Create an input archive

      iarchive(i_i, i_f, i_d, i_s);  // Read the data from the archive
    }
    /// [binary_archive]
    CHECK(i_i == o_i);
    CHECK(i_f == o_f);
    CHECK(i_d == o_d);
    CHECK(i_s == o_s);
  }

  using archives_list =
      tmpl::list<tmpl::pair<BinaryInputArchive, BinaryOutputArchive>>;

  template <template <typename, typename> class F>
  void run_for_archives() {
    tmpl::for_each<archives_list>([](auto t) {
      using ArchiveType = tmpl::type_from<decltype(t)>;
      using IArchive = typename ArchiveType::first_type;
      using OArchive = typename ArchiveType::second_type;
      F<IArchive, OArchive>::run();
    });
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.pod",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<Test_POD>();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.string",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestStringBasic>();
    run_for_archives<TestStringAll>();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.custom_structs",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestCustomStructs>();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.vector",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestVector>();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.tuple",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestTuple>();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.array",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestArray>();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.unordered_map",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestUnorderedMap>();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.tagged_tuple",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestTaggedTuple>();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.blaze_dynamic_vector",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestBlazeDynamicVector>();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.blaze_dynamic_matrix",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestBlazeDynamicMatrix>();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.blaze_dynamic_tensor",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestBlazeDynamicTensor>();
  }

}  // namespace
