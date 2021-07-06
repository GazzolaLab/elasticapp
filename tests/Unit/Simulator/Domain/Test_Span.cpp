//
// Created by Tejaswin Parthasarathy on 6/17/21.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Domain/Span.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {

  template <typename Numeric>
  struct TestSpan : public elastica::Span<Numeric> {
    using Parent = elastica::Span<Numeric>;

   public:
    //**************************************************************************
    /*!\brief Constructor for the Span class.
     *
     * \param arg_min Minimum value of the span
     * \param arg_max Maximum value of the span
     */
    explicit TestSpan(Numeric arg_min, Numeric arg_max) noexcept
        : Parent(arg_min, arg_max){};
    //**************************************************************************
    using Parent::Parent;
  };

  template <typename Numeric>
  using Span = TestSpan<Numeric>;

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Domain.Span",
      "[Unit][Simulator][Domain]") {
    // clang-format on

    // constructor
    Span<double> span{-2.0, 5.6};

    // subscript op
    CHECK(span[span.min_idx] == approx(-2.0));
    CHECK(span[span.max_idx] == approx(5.6));

    // copy
    auto new_span(span);

    // subscript op
    CHECK(new_span[new_span.min_idx] == approx(-2.0));
    CHECK(new_span[new_span.max_idx] == approx(5.6));

    span[span.min_idx] = -4.0;
    span[span.max_idx] = 11.2;

    // const subscript
    auto&& const_span = cpp17::as_const(span);
    CHECK(const_span[const_span.min_idx] == approx(-4.0));
    CHECK(const_span[const_span.max_idx] == approx(11.2));

    // assignment
    new_span = const_span;
    CHECK(new_span[new_span.min_idx] == approx(-4.0));
    CHECK(new_span[new_span.max_idx] == approx(11.2));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Domain.Span.make_span",
      "[Unit][Simulator][Domain]") {
    // clang-format on

    /// [make_span_eg]
    // with arguments in proper order
    auto span = elastica::make_span(-2.0, 5.6);
    // with arugments in reversed order, corrected by make_span()
    auto new_span = elastica::make_span(5.6, -2.0);
    /// [make_span_eg]

    static_assert(cpp17::is_same_v<decltype(span), elastica::Span<double>>,
                  "meta failure");
    CHECK(span[span.min_idx] == approx(-2.0));
    CHECK(span[span.max_idx] == approx(5.6));

    static_assert(cpp17::is_same_v<decltype(new_span), elastica::Span<double>>,
                  "meta failure");
    CHECK(new_span[new_span.min_idx] == approx(-2.0));
    CHECK(new_span[new_span.max_idx] == approx(5.6));
  }

}  // namespace