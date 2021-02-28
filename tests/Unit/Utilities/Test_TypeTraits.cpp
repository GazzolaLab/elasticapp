#include <map>
#include <memory>
#include <tuple>
#include <vector>

#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits.hpp"

/// \cond HIDDEN_SYMBOLS
class A {
 public:
  int operator()() { return 1; }
  A() = delete;
  std::unique_ptr<double> get_clone() const {
    return std::make_unique<double>();
  }
  int size() const { return 2; }
};

class BClassInTestTypeTraits {
 public:
  double operator()(const int /* unused */, const double /* unused */) {
    return 0.;
  }
  std::unique_ptr<double> clone() const { return std::make_unique<double>(); }
};

class CClassInTestTypeTraits {};

class D;

std::ostream& operator<<(std::ostream& os, const D&) noexcept;

bool operator==(const CClassInTestTypeTraits&, const CClassInTestTypeTraits&);

bool operator!=(const CClassInTestTypeTraits&, const CClassInTestTypeTraits&);

class EClassInTestTypeTraits : public std::vector<float> {};
/// \endcond


/// [is_contained_example]
static_assert(tt::is_contained_v<int, char, bool, float, double, int>,
              "Failed testing type trait is_contained");
static_assert(tt::is_contained_v<A, char, float, A, BClassInTestTypeTraits>,
              "Failed testing type trait is_contained");
static_assert(not tt::is_contained_v<int, char, bool, float, double>,
              "Failed testing type trait is_contained");
static_assert(
    not tt::is_contained_v<A, char, bool, float, BClassInTestTypeTraits, D>,
    "Failed testing type trait is_contained");
/// [is_contained_example]

/// [is_all_unique_example]
static_assert(tt::is_all_unique_v<int, char, bool, float, double>,
              "Failed testing type trait is_all_unique");
static_assert(tt::is_all_unique_v<A, BClassInTestTypeTraits,
                                  CClassInTestTypeTraits, float, D>,
              "Failed testing type trait is_all_unique");
static_assert(not tt::is_all_unique_v<int, char, bool, float, double, int>,
              "Failed testing type trait is_all_unique");
static_assert(
    not tt::is_all_unique_v<A, BClassInTestTypeTraits, CClassInTestTypeTraits,
                            float, A, BClassInTestTypeTraits, D>,
    "Failed testing type trait is_all_unique");
/// [is_all_unique_example]

/// [is_templated_base_example]
static_assert(tt::is_templated_base_of_v<std::vector, EClassInTestTypeTraits>,
              "Failed testing type trait is_templated_base");
static_assert(
    tt::is_templated_base_of_t<std::vector, EClassInTestTypeTraits>::value,
    "Failed testing type trait is_templated_base");
static_assert(
    not tt::is_templated_base_of_v<std::vector, BClassInTestTypeTraits>,
    "Failed testing type trait is_templated_base");
static_assert(
    not tt::is_templated_base_of_t<std::vector, CClassInTestTypeTraits>::value,
    "Failed testing type trait is_templated_base");
/// [is_templated_base_example]

/// [is_iterable_example]
static_assert(tt::is_iterable_v<std::vector<double>>,
              "Failed testing type trait is_iterable");
static_assert(tt::is_iterable_t<std::vector<double>>::value,
              "Failed testing type trait is_iterable");
static_assert(not tt::is_iterable_v<double>,
              "Failed testing type trait is_iterable");
static_assert(not tt::is_iterable_t<double>::value,
              "Failed testing type trait is_iterable");
/// [is_iterable_example]

/// [is_a_example]
static_assert(tt::is_a_v<std::vector, std::vector<double>>,
              "Failed testing type trait is_a<vector>");
static_assert(not tt::is_a_t<std::vector, double>::value,
              "Failed testing type trait is_a<vector>");
static_assert(tt::is_a_v<std::vector, std::vector<D>>,
              "Failed testing type trait is_a<vector>");

static_assert(tt::is_a_v<std::unique_ptr, std::unique_ptr<double>>,
              "Failed testing type trait is_a<unique_ptr>");
static_assert(
    tt::is_a_v<std::unique_ptr, std::unique_ptr<CClassInTestTypeTraits>>,
    "Failed testing type trait is_a<unique_ptr>");
static_assert(not tt::is_a_v<std::unique_ptr, std::shared_ptr<double>>,
              "Failed testing type trait is_a<unique_ptr>");
static_assert(not tt::is_a_v<std::unique_ptr, CClassInTestTypeTraits>,
              "Failed testing type trait is_a<unique_ptr>");

static_assert(tt::is_a_v<std::shared_ptr, std::shared_ptr<double>>,
              "Failed testing type trait is_a<shared_ptr>");
static_assert(
    tt::is_a_v<std::shared_ptr, std::shared_ptr<CClassInTestTypeTraits>>,
    "Failed testing type trait is_a<shared_ptr>");
static_assert(not tt::is_a_v<std::shared_ptr, std::unique_ptr<double>>,
              "Failed testing type trait is_a<shared_ptr>");

static_assert(tt::is_a_v<std::tuple, std::tuple<int, double, A>>,
              "Failed testing type trait is_a");
static_assert(tt::is_a_v<std::vector, std::vector<A>>,
              "Failed testing type trait is_a");
/// [is_a_example]

/// [is_maplike_example]
/*
 * TODO : Modify to an instance of unordered map if using
 * one in the future
 */
static_assert(tt::is_maplike_v<std::map<int, double>>,
              "Failed testing type trait is_maplike");
static_assert(tt::is_maplike_t<std::map<int, BClassInTestTypeTraits>>::value,
              "Failed testing type trait is_maplike");
static_assert(not tt::is_maplike_v<std::vector<CClassInTestTypeTraits>>,
              "Failed testing type trait is_maplike");
static_assert(not tt::is_maplike_t<D>::value,
              "Failed testing type trait is_maplike");
/// [is_maplike_example]

/// [is_streamable_example]
static_assert(tt::is_streamable_v<std::ostream, D>,
              "Failed testing type trait is_streamable");
static_assert(tt::is_streamable_t<std::ostream, D>::value,
              "Failed testing type trait is_streamable");
static_assert(not tt::is_streamable_v<std::ostream, CClassInTestTypeTraits>,
              "Failed testing type trait is_streamable");
static_assert(
    not tt::is_streamable_t<std::ostream, CClassInTestTypeTraits>::value,
    "Failed testing type trait is_streamable");
/// [is_streamable_example]