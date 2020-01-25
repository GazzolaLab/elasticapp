#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

#include "Utilities/Policies/NoDelete.hpp"
#include "Utilities/PtrVector.hpp"
#include "tests/Unit/TestingFramework.hpp"

using namespace elastica;
using namespace std;

struct Parent {
  Parent() = default;
  void print() { std::cout << "Hi" << std::endl; };
  virtual ~Parent() = default;
};

#define MAKE_CHILD_STRUCT(N, T)  \
  struct N : public Parent {     \
    using MemberType = T;        \
    N() : Parent(){};            \
    ~N() = default;              \
    virtual string name() {      \
      std::stringstream ss;      \
      ss << "Hello from " << #N; \
      return ss.str();           \
    }                            \
  };

MAKE_CHILD_STRUCT(FirstTestStruct, double)
MAKE_CHILD_STRUCT(SecondTestStruct, double)

using BasicTypes = tuple<int, char, float, unsigned int>;
using ConstructionTypes = tuple<int, char, float, SecondTestStruct>;

TEMPLATE_LIST_TEST_CASE("Unit.Utilities.PtrVector.DefaultConstructors",
                        "[Unit][Utilities]", ConstructionTypes) {
  PtrVector<TestType, NoDelete> pv;
  CHECK(pv.size() == 0UL);
  CHECK(pv.capacity() == 0UL);

  size_t vsize = 10UL;
  PtrVector<TestType> pv_constructed(vsize);
  CHECK(pv_constructed.size() == 0);
  CHECK(pv_constructed.capacity() == vsize);
}

ELASTICA_TEST_CASE("Unit.Utilities.PtrVector.PushBack", "[Unit][Utilities]") {
  PtrVector<int, NoDelete> pv(5);
  int* resources = new int[10];

  std::iota(resources, resources + 10UL, 0.0);

  for (size_t i = 0UL; i < 5UL; ++i)
    pv.push_back(resources + i);

  CHECK(pv.size() == 5);
  CHECK(pv.capacity() == 5);

  pv.push_back(resources + 5UL);
  CHECK(pv.size() == 6);
  CHECK(pv.capacity() >= 6);

  //  for (size_t i = 0UL; i < 6UL; ++i){
  //    CHECK(resources[i] == *pv[i]);
  //  }
  //  CHECK(resources[5UL] == *pv[6UL]);

  delete[] resources;
}

ELASTICA_TEST_CASE("Unit.Utilities.PtrVector.CopyConstructor",
                   "[Unit][Utilities]") {
  using TestType = double;
  constexpr std::size_t test_capacity = 5UL;
  TestType* resources = new TestType[test_capacity];
  std::iota(resources, resources + test_capacity, static_cast<TestType>(0.0));

  PtrVector<TestType> pv(test_capacity);
  for (std::size_t i = 0UL; i < test_capacity; ++i)
    pv.push_back(resources + i);

  PtrVector<TestType> pv3(pv);
  CHECK(pv3.size() == test_capacity);
  CHECK(pv3.capacity() == test_capacity);

  for (std::size_t i = 0UL; i < test_capacity; ++i) {
    CHECK(pv3[i] == (resources + i));
    CHECK(*pv3[i] == *pv[i]);
  }

  delete[] resources;
}

ELASTICA_TEST_CASE("Unit.Utilities.PtrVector.CoercedConstructor",
                   "[Unit][Utilities]") {
  using FirstType = FirstTestStruct;
  using SecondType = SecondTestStruct;

  constexpr size_t test_capacity = 5UL;
  auto* first_types = new FirstType[test_capacity];

  PtrVector<FirstType> pvf(test_capacity);
  for (size_t i = 0UL; i < test_capacity; ++i) {
    pvf.push_back(first_types + i);
  }

  PtrVector<Parent> pvs(pvf);

  for (size_t i = 0UL; i < test_capacity; ++i) {
    if (auto* ft = dynamic_cast<FirstType*>(pvs[i]))
      CHECK(ft->name() == std::string("Hello from FirstTestStruct"));
  }

  delete[] first_types;
}

TEMPLATE_LIST_TEST_CASE("Unit.Utilities.PtrVector.CopyAssignmentOperator",
                        "[Unit][Utilities]", BasicTypes) {
  //  using TestType = unsigned int;
  constexpr std::size_t test_capacity = 5UL;
  TestType* resources = new TestType[test_capacity];
  std::iota(resources, resources + test_capacity, static_cast<TestType>(0.0));

  PtrVector<TestType> pv(test_capacity);
  for (std::size_t i = 0UL; i < test_capacity; ++i)
    pv.push_back(resources + i);

  PtrVector<TestType> pv3;
  CHECK(pv3.size() == 0UL);
  CHECK(pv3.capacity() == 0UL);

  pv3 = pv;
  CHECK(pv3.size() == 5UL);
  CHECK(pv3.capacity() == 5UL);

  for (std::size_t i = 0UL; i < test_capacity; ++i) {
    CHECK(pv3[i] == (resources + i));
    CHECK(*pv3[i] == *pv[i]);
  }
  delete[] resources;
}

ELASTICA_TEST_CASE("Unit.Utilities.PtrVector.CoercedAssignmentOperator",
                   "[Unit][Utilities]") {
  using FirstType = FirstTestStruct;

  constexpr size_t test_capacity = 5UL;
  auto* first_types = new FirstType[test_capacity];

  PtrVector<FirstType> pvf(test_capacity);
  for (size_t i = 0UL; i < test_capacity; ++i) {
    pvf.push_back(first_types + i);
  }

  PtrVector<Parent> pvs(test_capacity);
  pvs = pvf;

  for (size_t i = 0UL; i < test_capacity; ++i) {
    if (auto* ft = dynamic_cast<FirstType*>(pvs[i]))
      CHECK(ft->name() == std::string("Hello from FirstTestStruct"));
  }

  delete[] first_types;
}

ELASTICA_TEST_CASE("Unit.Utilities.PtrVector.Empty", "[Unit][Utilities]") {
  using TestType = double;
  constexpr std::size_t test_capacity = 5UL;
  TestType* resources = new TestType[test_capacity];
  std::iota(resources, resources + test_capacity, static_cast<TestType>(0.0));

  PtrVector<TestType> pv(test_capacity);
  CHECK(pv.empty());

  for (std::size_t i = 0UL; i < test_capacity; ++i)
    pv.push_back(resources + i);

  CHECK(!pv.empty());
  delete[] resources;
}

ELASTICA_TEST_CASE("Unit.Utilities.PtrVector.FrontBack", "[Unit][Utilities]") {
  using TestType = float;
  constexpr std::size_t test_capacity = 5UL;
  TestType* resources = new TestType[test_capacity];
  std::iota(resources, resources + test_capacity, static_cast<TestType>(0.0));

  PtrVector<TestType> pv(test_capacity);

  for (std::size_t i = 0UL; i < test_capacity; ++i)
    pv.push_back(resources + i);

  // Refs
  CHECK(0.0f == *pv.front());
  CHECK(*pv.back() == static_cast<TestType>(test_capacity - 1));
  delete[] resources;
}

ELASTICA_TEST_CASE("Unit.Utilities.PtrVector.BeginEnd", "[Unit][Utilities]") {
  using TestType = float;
  constexpr std::size_t test_capacity = 5UL;
  TestType* resources = new TestType[test_capacity];
  std::iota(resources, resources + test_capacity, static_cast<TestType>(0.0));

  PtrVector<TestType> pv(test_capacity);
  for (std::size_t i = 0UL; i < test_capacity; ++i)
    pv.push_back(resources + i);

  std::size_t i = 0UL;
  // Also testing the iterator
  for (typename PtrVector<TestType>::Iterator it = pv.begin(); it != pv.end();
       ++it, ++i) {
    CHECK(**it == *(resources + i));
  }

  i = 0UL;
  // Also testing the iterator
  for (PtrVector<TestType>::ConstIterator it = pv.cbegin(); it != pv.cend();
       ++it, ++i) {
    CHECK(**it == *(resources + i));
  }
  delete[] resources;
}

#define MAKE_POP_RELEASE_TEST_CASE(m)                                          \
  using TestType = int;                                                        \
  constexpr std::size_t test_capacity = 5UL;                                   \
  TestType* resources = new TestType[test_capacity];                           \
  std::iota(resources, resources + test_capacity, static_cast<TestType>(0.0)); \
  PtrVector<int, NoDelete> pv(test_capacity);                                  \
  for (std::size_t i = 0UL; i < test_capacity; ++i)                            \
    pv.push_back(resources + i);                                               \
  CHECK(pv.size() == test_capacity);                                           \
  CHECK(pv.capacity() == test_capacity);                                       \
  pv.m();                                                                      \
  pv.m();                                                                      \
  CHECK(pv.size() == (test_capacity - 2));                                     \
  CHECK(pv.capacity() == test_capacity);                                       \
  delete[] resources;

ELASTICA_TEST_CASE("Unit.Utilities.PtrVector.PopBack", "[Unit][Utilities]") {
  MAKE_POP_RELEASE_TEST_CASE(pop_back);
}

ELASTICA_TEST_CASE("Unit.Utilities.PtrVector.ReleaseBack",
                   "[Unit][Utilities]") {
  MAKE_POP_RELEASE_TEST_CASE(release_back);
}

ELASTICA_TEST_CASE("Unit.Utilities.PtrVector.Erase",
                   "[Unit][Utilities]") {
  using TestType = float;
  constexpr std::size_t test_capacity = 5UL;
  TestType* resources = new TestType[test_capacity];
  std::iota(resources, resources + test_capacity, static_cast<TestType>(0.0));

  PtrVector<TestType> pv(test_capacity);
  for (std::size_t i = 0UL; i < test_capacity; ++i)
    pv.push_back(resources + i);

  PtrVector<TestType>::ConstIterator it = pv.cbegin();
  constexpr std::size_t offset = 3UL;
  it += 3UL;

  auto new_it = pv.erase(it);

  for (std::size_t i = 0UL; i < test_capacity; ++i) {
    if (i == offset)
      break;
    CHECK(pv[i] == (resources + i));
  }
  CHECK(pv[offset] != (resources + offset));
  CHECK(pv[offset] == (resources + offset + 1UL));
  CHECK(*new_it == pv[offset]);


  delete[] resources;
}
