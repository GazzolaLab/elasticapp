//
// Created by Tejaswin Parthasarathy on 5/25/21.
//

// [has_equivalence_example]
namespace {
  class A {};
}  // namespace

namespace TestHelpers {
  class C {};
  bool operator==(const C&, const C&);
}  // namespace TestHelpers

static_assert(not tt::has_equivalence<A>::value,
              "Failed testing type trait has_equivalence");
// [has_equivalence_example]

// [has_equivalence_t_example]
static_assert(tt::has_equivalence<TestHelpers::C>::value,
              "Failed testing type trait has_equivalence");
static_assert(tt::has_equivalence_t<TestHelpers::C>::value,
              "Failed testing type trait has_equivalence");
// [has_equivalence_t_example]

// [has_equivalence_v_example]
static_assert(tt::has_equivalence<TestHelpers::C>::value,
              "Failed testing type trait has_equivalence");
static_assert(tt::has_equivalence_v<TestHelpers::C>,
              "Failed testing type trait has_equivalence");
// [has_equivalence_v_example]
