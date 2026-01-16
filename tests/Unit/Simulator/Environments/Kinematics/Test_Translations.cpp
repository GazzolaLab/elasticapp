//
// Created by Tejaswin Parthasarathy on 5/19/22.
//

#include <functional>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/Kinematics/Translations.hpp"
#include "Simulator/Frames.hpp"

namespace {

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Environments.Kinematics.TranslationExpr",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    auto dummy_func = [](elastica::real_t) { return 2; };
    using ::elastica::kinematics::detail::TranslationExpr;

    using XT =
        TranslationExpr<elastica::EulerianFrame::X, decltype(dummy_func)>;
    using YT =
        TranslationExpr<elastica::EulerianFrame::Y, decltype(dummy_func)>;
    using ZT =
        TranslationExpr<elastica::EulerianFrame::Z, decltype(dummy_func)>;

    XT xt{dummy_func};
    ::elastica::Time time_v{0.0};
    CHECK(xt(time_v) == 2);

    YT yt{dummy_func};
    CHECK(yt(time_v) == 2);

    ZT zt{dummy_func};
    CHECK(zt(time_v) == 2);

    // check interop with functional
    using Func = std::function<int(::elastica::real_t)>;
    using FXT = TranslationExpr<elastica::EulerianFrame::X, Func>;
    FXT fxt{xt};
    CHECK(fxt(time_v) == 2);

    using FYT = TranslationExpr<elastica::EulerianFrame::Y, Func>;
    // trivially-movable
    FYT fyt{std::move(yt)};
    CHECK(fyt(time_v) == 2);

    auto zzt = zt;
    CHECK(zzt(time_v) == 2);
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Environments.Kinematics.TranslationKinematics",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    auto p = [](elastica::real_t) { return 2; };
    auto v = [](elastica::real_t) { return 0; };

    using ::elastica::kinematics::detail::TranslationExpr;
    using ::elastica::kinematics::detail::TranslationKinematics;

    using P = TranslationExpr<elastica::EulerianFrame::X, decltype(p)>;
    using V = TranslationExpr<elastica::EulerianFrame::X, decltype(v)>;
    using TK = TranslationKinematics<elastica::EulerianFrame::X, decltype(p),
                                     decltype(v)>;
    TK tk{P(p), V(v)};
    ::elastica::Time time_v{0.0};
    CHECK(tk(time_v) == 2);
    CHECK(tk.derivative(time_v) == 0);

    using Func = std::function<int(::elastica::real_t)>;
    using FTK = TranslationKinematics<elastica::EulerianFrame::X, Func, Func>;

    FTK ftk{tk};
    CHECK(ftk(time_v) == 2);
    CHECK(ftk.derivative(time_v) == 0);

    auto r = elastica::kinematics::detail::translate_along<
        elastica::EulerianFrame::X>(std::move(p), std::move(v));
    CHECK(r(time_v) == 2);
    CHECK(r.derivative(time_v) == 0);

    FTK aftk{tk};
    aftk = std::move(ftk);
    CHECK(aftk(time_v) == 2);
    CHECK(aftk.derivative(time_v) == 0);
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Environments.Kinematics.GroupedTranslationKinematics",
      "[Unit][Simulator][Environments]") {
    // clang-format on
    auto p = [](elastica::real_t) { return 2; };
    auto p1 = [](elastica::real_t) { return 3; };
    auto p2 = [](elastica::real_t) { return 4; };
    auto v = [](elastica::real_t) { return 0; };

    using ::elastica::kinematics::detail::GroupedTranslationKinematics;
    using ::elastica::kinematics::detail::TranslationKinematics;

    auto x = elastica::kinematics::detail::translate_along<
        elastica::EulerianFrame::X>(p, v);
    auto y = elastica::kinematics::detail::translate_along<
        elastica::EulerianFrame::Y>(p1, v);
    auto z = elastica::kinematics::detail::translate_along<
        elastica::EulerianFrame::Z>(p2, v);

    GroupedTranslationKinematics<decltype(x), decltype(y), decltype(z)> g(x, y,
                                                                          z);

    // Clang-Tidy: Static member accessed through instance
    static_assert(g.size() == 3UL, "Meta-failure");  // NOLINT

    // checks for get_kinematics

    // Clang-Tidy: Static member accessed through instance
    static_assert(elastica::detail::cast_along(
                      elastica::kinematics::detail::get_kinematics<
                          elastica::EulerianFrame::X>(g)
                          .Direction) ==
                      elastica::detail::cast_along(elastica::EulerianFrame::X),
                  "Meta-failure");
    static_assert(elastica::detail::cast_along(
                      elastica::kinematics::detail::get_kinematics<
                          elastica::EulerianFrame::Y>(g)
                          .Direction) ==
                      elastica::detail::cast_along(elastica::EulerianFrame::Y),
                  "Meta-failure");
    static_assert(elastica::detail::cast_along(
                      elastica::kinematics::detail::get_kinematics<
                          elastica::EulerianFrame::Z>(g)
                          .Direction) ==
                      elastica::detail::cast_along(elastica::EulerianFrame::Z),
                  "Meta-failure");

    static_assert(elastica::detail::cast_along(
                      elastica::kinematics::detail::get_kinematics<
                          elastica::EulerianFrame::X>(std::as_const(g))
                          .Direction) ==
                      elastica::detail::cast_along(elastica::EulerianFrame::X),
                  "Meta-failure");
    static_assert(elastica::detail::cast_along(
                      elastica::kinematics::detail::get_kinematics<
                          elastica::EulerianFrame::Y>(std::as_const(g))
                          .Direction) ==
                      elastica::detail::cast_along(elastica::EulerianFrame::Y),
                  "Meta-failure");
    static_assert(elastica::detail::cast_along(
                      elastica::kinematics::detail::get_kinematics<
                          elastica::EulerianFrame::Z>(std::as_const(g))
                          .Direction) ==
                      elastica::detail::cast_along(elastica::EulerianFrame::Z),
                  "Meta-failure");

    using Func = std::function<int(::elastica::real_t)>;
    using FX = TranslationKinematics<elastica::EulerianFrame::X, Func, Func>;
    using TestGroupedTranslationKinematics =
        GroupedTranslationKinematics<FX, decltype(y), decltype(z)>;
    // test copyable to func
    TestGroupedTranslationKinematics tg{g};

    // Test coyable even if order is changed
    using FY = TranslationKinematics<elastica::EulerianFrame::Y, Func, Func>;
    using AT = GroupedTranslationKinematics<FX, FY, decltype(z)>;
    AT atg{g};

    using FZ = TranslationKinematics<elastica::EulerianFrame::Z, Func, Func>;
    using YAT = GroupedTranslationKinematics<FX, FY, FZ>;
    YAT ya_atg{g};
    YAT another_atg{g};
    another_atg = ya_atg;

    SECTION("Compute positions") {
      ::elastica::Time time_v{0.0};

      auto check_access = [=](auto& k) {
        CHECK(k.template compute_position_along<elastica::EulerianFrame::X>(
                  time_v) == 2);
        CHECK(k.template compute_position_along<elastica::EulerianFrame::Y>(
                  time_v) == 3);
        CHECK(k.template compute_position_along<elastica::EulerianFrame::Z>(
                  time_v) == 4);
      };
      check_access(g);
      check_access(std::as_const(g));
      check_access(tg);
      check_access(std::as_const(tg));
      check_access(atg);
      check_access(std::as_const(atg));
      check_access(another_atg);
      check_access(std::as_const(another_atg));
    }

    SECTION("Compute velocities") {
      ::elastica::Time time_v{0.0};
      auto check_access = [=](auto& k) {
        CHECK(k.template compute_velocity_along<elastica::EulerianFrame::X>(
                  time_v) == 0);
        CHECK(k.template compute_velocity_along<elastica::EulerianFrame::Y>(
                  time_v) == 0);
        CHECK(k.template compute_velocity_along<elastica::EulerianFrame::Z>(
                  time_v) == 0);
      };
      check_access(g);
      check_access(std::as_const(g));
      check_access(tg);
      check_access(std::as_const(tg));
      check_access(atg);
      check_access(std::as_const(atg));
      check_access(another_atg);
      check_access(std::as_const(another_atg));
    }
  }

  ELASTICA_TEST_CASE("Unit.Simulator.Environments.Kinematics.mark",
                     "[Unit][Simulator][Environments]") {
    /// [mark_position_example]
    ::elastica::kinematics::mark_position(
        [](::elastica::real_t time_v) -> ::elastica::real_t {
          return 1.0 + 0.01 * time_v;
        });

    struct Squared {
      ::elastica::real_t operator()(::elastica::real_t time_v) const {
        return 0.1 * time_v * time_v;
      }
    };
    ::elastica::kinematics::mark_position(Squared{});
    /// [mark_position_example]

    /// [mark_velocity_example]
    ::elastica::kinematics::mark_velocity(
        [](::elastica::real_t /*time_v*/) -> ::elastica::real_t {
          return 0.01;
        });

    struct SquaredDerivative {
      ::elastica::real_t operator()(::elastica::real_t time_v) const {
        return 0.1 * 2.0 * time_v;
      }
    };
    ::elastica::kinematics::mark_velocity(SquaredDerivative{});
    /// [mark_velocity_example]

    /// [translate_along_eg]
    ::elastica::kinematics::translate_along<::elastica::EulerianFrame::X>(
        elastica::kinematics::mark_position(Squared{}),
        elastica::kinematics::mark_velocity(SquaredDerivative{}));
    ::elastica::kinematics::translate_along<::elastica::EulerianFrame::Y>(
        elastica::kinematics::mark_velocity(SquaredDerivative{}),
        elastica::kinematics::mark_position(Squared{}));
    /// [translate_along_eg]

    /// [make_translations_eg]
    namespace ek = ::elastica::kinematics;
    auto tX = ek::translate_along<::elastica::EulerianFrame::X>(
        ek::mark_position(Squared{}), ek::mark_velocity(SquaredDerivative{}));
    auto tY = ek::translate_along<::elastica::EulerianFrame::Y>(
        ek::mark_position(Squared{}), ek::mark_velocity(SquaredDerivative{}));
    auto tZ = ek::translate_along<::elastica::EulerianFrame::Z>(
        ek::mark_position(Squared{}), ek::mark_velocity(SquaredDerivative{}));

    auto translations = ek::make_translations(tX, tY, tZ);
    // ordering does not matter
    ek::make_translations(tZ, tY, tX);
    /// [make_translations_eg]

    auto check_access = [](auto& k) {
      ::elastica::Time time_v{1.0};
      CHECK(k.template compute_position_along<elastica::EulerianFrame::X>(
                time_v) == 0.1);
      CHECK(k.template compute_position_along<elastica::EulerianFrame::Y>(
                time_v) == 0.1);
      CHECK(k.template compute_position_along<elastica::EulerianFrame::Z>(
                time_v) == 0.1);
      CHECK(k.template compute_velocity_along<elastica::EulerianFrame::X>(
                time_v) == 0.2);
      CHECK(k.template compute_velocity_along<elastica::EulerianFrame::Y>(
                time_v) == 0.2);
      CHECK(k.template compute_velocity_along<elastica::EulerianFrame::Z>(
                time_v) == 0.2);
    };
    check_access(translations);
    check_access(std::as_const(translations));
  }

}  // namespace