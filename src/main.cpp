#include <iostream>
#include <new>
#include <string>
//#include <vector>

//#include "tbb/parallel_for.h"

//#include "blaze/Math.h"

#include "DataStructures/Block.hpp"
#include "DataStructures/StorageTraits.hpp"
#include "DataStructures/TaggedTuple.hpp"
#include "Utilities/DefineTypes.h"
#include "Utilities/StdHelpers.hpp"

// using blaze::StaticVector;
// using blaze::DynamicVector;
// template <typename ...Args>
// void __attribute__((noinline)) test_valgrind(TaggedTuple<Args...>& tt){
////  float* a = new float[100];
////  for(int i = 0; i < 100; ++i) a[i] = i*2.1;
//  std::get<Tags::SimpleTag>(tt) = 3.0;
//}
//
int main(int argc, char* argv[]) {
  std::string b("Hello World!");
  std::cout << b << std::endl;
  //  std::cout << std::boolalpha <<
  //  tt::is_all_unique_v<Tags::Position<3>,Tags::Position<3>>;

  //  TaggedTuple<Tags::Position, Tags::Velocity> tt;
  //  std::get<Tags::Position<3,10>>(tt) = std::vector<real>{2.0,3.0,4.0,5.0};
  //  TaggedTuple<Tags::SimpleTag, Tags::BooTag> tt;
  //  test_valgrind(tt);
  //  std::get<Tags::SimpleTag>(tt) = 3.0;
  //  std::cout << tt;
  //  std::cout << "Whathta";

  std::cout << std::boolalpha << Tags::is_container_tag_v<Tags::Position>;

  Blocks::Block<Groups::GenericBodyGroup,
                Blocks::BlockData<3, 9, storage_detail::StorageOption::Static>>
      sb;
  TaggedTuple<Tags::SimplePODTag> tt;
  using myt = decltype(tuples::get<Tags::Position, Tags::Velocity>(sb));
  using yot = decltype(std::get<Tags::Position>(sb));
  //  Blocks::Block<Groups::GenericBodyGroup, Blocks::BlockData<3, 9,
  //  storage_detail::StorageOption::Static>> sb; using my_type =
  //  decltype(std::get<Tags::Position>(sb) * std::get<Tags::OneDimension>(sb));
  ////  BlockTuple<BlockMetaData<3,20>, Tags::Position, Tags::Velocity> sb;
  ////  std::get<Tags::Position>(sb) = std::vector<double>{1.0,2.0,3.0,4.0};
  ////  std::cout << std::get<Tags::Position>(sb);
  ////  std::get<Tags::Velocity>(sb) = std::vector<double>{5.0,2.0,2.0,4.0};
  //  std::cout << sb;

  return 0;
}
