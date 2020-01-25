#include <iostream>
#include <new>
#include <string>
//#include <vector>

//#include "tbb/parallel_for.h"

//#include "blaze/Math.h"

#include "DataStructures/Block.hpp"
#include "DataStructures/StorageTraits.hpp"
#include "DataStructures/TaggedTuple.hpp"
#include "Time/History.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/Logging.hpp"
#include "Utilities/Singleton.hpp"
#include "Utilities/StdHelpers.hpp"

/*
int main(int argc, char* argv[]) {
  constexpr size_t Dimension = 3;
  constexpr size_t BlockSize = 10;
  using BlockMetaData =
      Blocks::BlockData<Dimension, BlockSize,
                        storage_detail::StorageOption::Static>;
  using toy_block = Blocks::Block<Groups::GenericBodyGroup, BlockMetaData>;
  toy_block sb;
  std::cout << std::boolalpha << Blocks::is_valid_block_v<toy_block> << '\n';
  TimeSteppers::History<toy_block,
                        tuples::item_data_type<Tags::Position, BlockMetaData>,
                        tuples::item_data_type<Tags::Velocity, BlockMetaData>>
      history;
  std::cout << history.capacity() + 1;
  //
  //  std::get<Tags::Position>(sb),
  //                        std::get<Tags::Velocity>(sb)>
  //      h;
  //  TaggedTuple<Tags::SimplePODTag> tt;
  //  using myt = decltype(tuples::get<Tags::Position, Tags::Velocity>(sb));
  //  using yot = decltype(std::get<Tags::Position>(sb));

  return 0;
}
*/

using namespace elastica;

class TestDep : public Singleton<TestDep> {
 private:
  explicit TestDep(){};

 public:
  ~TestDep(){};

 private:
  BEFRIEND_SINGLETON;
};

class TestSingle : private Singleton<TestSingle, TestDep> {
 private:
  explicit TestSingle() { a = 2; };

 public:
  ~TestSingle(){};
  int a;
  friend inline std::shared_ptr<TestSingle> TheTestSingle();

 private:
  BEFRIEND_SINGLETON;
};

inline std::shared_ptr<TestSingle> TheTestSingle() {
  return TestSingle::instance();
}

int main(int argc, char* argv[]) {
  auto ts = TheTestSingle();
  LOG_INFO_SECTION(log) { log << "A IS " << ts->a << "BOIII"; }
  LOG_ERROR_SECTION(log) { log << "Thick BOIII"; }
}
