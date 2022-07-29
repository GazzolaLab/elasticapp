
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
#include "detail/Test_Map.hpp"
#include "detail/Test_NVP.hpp"
#include "detail/Test_POD.hpp"
#include "detail/Test_String.hpp"
#include "detail/Test_TaggedTuple.hpp"
#include "detail/Test_Timer.hpp"
#include "detail/Test_Tuple.hpp"
#include "detail/Test_UnorderedMap.hpp"
#include "detail/Test_Utility.hpp"
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
  {
    /// [custom_serialize_usage]
    using namespace elastica::io;
    std::stringstream ss;  // any stream can be used
    MyImportantInfo o_info{2, 2.0};
    {
      BinaryOutputArchive oarchive(ss);  //  Create an output archive
      oarchive(o_info);                  // Write the data to the archive
    }
    /// [custom_serialize_usage]
    CAPTURE(ss.str());
    CHECK_FALSE(ss.str().empty());
  }

#if defined(ELASTICA_USE_YAMLCPP)
  // same usage with input archives for loading
  {
    /// [custom_yaml_serialize_usage]
    using namespace elastica::io;
    std::stringstream ss;  // any stream can be used
    MyImportantInfo o_info{2, 2.0};
    {
      YAMLOutputArchive oarchive(ss);  //  Create an output archive
      oarchive(o_info);                // Write the data to the archive
    }
    /// [custom_yaml_serialize_usage]

    CAPTURE(ss.str());
    CHECK_FALSE(ss.str().empty());
  }
#endif  // defined(ELASTICA_USE_YAMLCPP)
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

  using BinaryArchives = tmpl::pair<BinaryInputArchive, BinaryOutputArchive>;
  using YAMLArchives =
      tmpl::pair<YAMLInputArchive,
                 YAMLOutputArchive>;  // symbols are available always
  using archives_list = tmpl::list<BinaryArchives>;

  template <template <typename, typename> class F, typename ArchiveType>
  void run_for_archive(tmpl::type_<ArchiveType>) {
    using IArchive = typename ArchiveType::first_type;
    using OArchive = typename ArchiveType::second_type;
    SECTION(pretty_type::short_name<IArchive>() + " & " +
            pretty_type::short_name<OArchive>()) {
      F<IArchive, OArchive>::run();
    }
  }

  template <template <typename, typename> class F>
  void run_for_archive(tmpl::type_<YAMLArchives>) {
#if defined(ELASTICA_USE_YAMLCPP)
    using IArchive = typename YAMLArchives::first_type;
    using OArchive = typename YAMLArchives::second_type;
    SECTION(pretty_type::short_name<IArchive>() + " & " +
            pretty_type::short_name<OArchive>()) {
      F<IArchive, OArchive>::run();
    }
#endif  // defined(ELASTICA_USE_YAMLCPP)
  }

  template <template <typename, typename> class F>
  void run_for_archives() {
    tmpl::for_each<archives_list>([](auto t) { run_for_archive<F>(t); });
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.not_no_op",
                     "[Unit][Simulator][IO][Serialization]") {
    test_custom_serialize();
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.pod",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<Test_POD>();
    test_custom_serialize();
    run_for_archive<Test_POD>(tmpl::type_<YAMLArchives>{});
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.string",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestStringBasic>();
    run_for_archives<TestStringAll>();

    run_for_archive<TestStringBasic>(tmpl::type_<YAMLArchives>{});
    // TODO : archiving of basic strings with different chars do not work
    // run_for_archive<TestStringAll>(tmpl::type_<YAMLArchives>{});
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.custom_structs",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestCustomStructs>();
    run_for_archive<TestCustomStructs>(tmpl::type_<YAMLArchives>{});
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.pair",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestUtility>();
    run_for_archive<TestUtility>(tmpl::type_<YAMLArchives>{});
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.nvp",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestNVP>();
    run_for_archive<TestNVP>(tmpl::type_<YAMLArchives>{});
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.vector",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestVector>();
    run_for_archive<TestVector>(tmpl::type_<YAMLArchives>{});
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.timer",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestTimer>();
    run_for_archive<TestTimer>(tmpl::type_<YAMLArchives>{});
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.tuple",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestTuple>();
    run_for_archive<TestTuple>(tmpl::type_<YAMLArchives>{});
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.array",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestArray>();
    run_for_archive<TestArray>(tmpl::type_<YAMLArchives>{});
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.unordered_map",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestUnorderedMap>();
    run_for_archive<TestUnorderedMap>(tmpl::type_<YAMLArchives>{});
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.map",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestMap>();
    run_for_archive<TestMap>(tmpl::type_<YAMLArchives>{});
  }

  ELASTICA_TEST_CASE("Unit.Simulator.IO.Serialize.tagged_tuple",
                     "[Unit][Simulator][IO][Serialization]") {
    run_for_archives<TestTaggedTuple>();
    run_for_archive<TestTaggedTuple>(tmpl::type_<YAMLArchives>{});
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
