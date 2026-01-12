//
// Created by Tejaswin Parthasarathy on 6/16/21.
//
#include <deque>
#include <list>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/BlockingPolicy.hpp"
#include "Utilities/IgnoreUnused.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"
//
#include "Options/ParseOptions.hpp"

namespace {

  using namespace elastica::configuration;

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.RestrictSize",
      "[Unit][Simulator][Configuration]") {
    // clang-format on

    // anything with a valid size
    using Block = std::vector<int>;
    RestrictSize<Block> restricter{20UL};

    Block b(10UL);
    // if size(vec) <, passes
    CHECK(restricter(b, 5UL));

    b.resize(18UL);
    CHECK(not restricter(b, 5UL));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.LooselyRestrictSize",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    // anything with a valid size
    using Block = std::vector<int>;
    LooselyRestrictSize<Block> restricter{20UL};

    Block b(10UL);
    // if size(vec) <, passes
    CHECK(restricter(b, 5UL));

    b.resize(18UL);
    CHECK(restricter(b, 5UL));

    b.resize(21UL);
    CHECK(not restricter(b, 5UL));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.AlwaysNew",
      "[Unit][Simulator][Configuration]") {
    // clang-format on

    // anything with a valid size
    using Block = std::vector<int>;
    AlwaysNew<Block> restricter{20UL};

    Block b(10UL);
    // if size(vec) <, passes
    CHECK(not restricter(b, 5UL));

    b.resize(18UL);
    CHECK(not restricter(b, 5UL));

    b.resize(21UL);
    CHECK(not restricter(b, 5UL));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.AlwaysSame",
      "[Unit][Simulator][Configuration]") {
    // clang-format on

    // anything with a valid size
    using Block = std::vector<int>;
    AlwaysSame<Block> restricter{20UL};

    Block b(10UL);
    // if size(vec) <, passes
    CHECK(restricter(b, 5UL));

    b.resize(18UL);
    CHECK(restricter(b, 5UL));

    b.resize(21UL);
    CHECK(restricter(b, 5UL));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.RestrictSizeAcrossBlockTypes",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using Restricter = AcrossBlockTypes<RestrictSize>;

    Restricter restricter{20UL};

    // anything with a valid size() method
    using FirstBlock = std::vector<int>;
    using SecondBlock = std::list<float>;

    tmpl::for_each<tmpl::list<FirstBlock, SecondBlock>>([&](auto v) {
      using Block = tmpl::type_from<decltype(v)>;
      Block b(10UL);
      // if size(vec) <, passes
      CHECK(restricter(b, 5UL));

      b.resize(18UL);
      CHECK(not restricter(b, 5UL));
    });
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.LooselyRestrictSizeAcrossBlockTypes",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using Restricter = AcrossBlockTypes<LooselyRestrictSize>;

    Restricter restricter{20UL};

    // anything with a valid size() method
    using FirstBlock = std::vector<int>;
    using SecondBlock = std::list<float>;

    tmpl::for_each<tmpl::list<FirstBlock, SecondBlock>>([&](auto v) {
      using Block = tmpl::type_from<decltype(v)>;
      Block b(10UL);
      // if size(vec) <, passes
      CHECK(restricter(b, 5UL));

      b.resize(18UL);
      CHECK(restricter(b, 5UL));

      b.resize(21UL);
      CHECK(not restricter(b, 5UL));
    });
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.AlwaysNewAcrossBlockTypes",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using Restricter = AcrossBlockTypes<AlwaysNew>;

    Restricter restricter{20UL};

    // anything with a valid size() method
    using FirstBlock = std::vector<int>;
    using SecondBlock = std::list<float>;

    tmpl::for_each<tmpl::list<FirstBlock, SecondBlock>>([&](auto v) {
      using Block = tmpl::type_from<decltype(v)>;

      Block b(10UL);
      // if size(vec) <, passes
      CHECK(not restricter(b, 5UL));

      b.resize(18UL);
      CHECK(not restricter(b, 5UL));

      b.resize(21UL);
      CHECK(not restricter(b, 5UL));
    });
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.AlwaysSameAcrossBlockTypes",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using Restricter = AcrossBlockTypes<AlwaysSame>;

    Restricter restricter{20UL};

    // anything with a valid size() method
    using FirstBlock = std::vector<int>;
    using SecondBlock = std::list<float>;

    tmpl::for_each<tmpl::list<FirstBlock, SecondBlock>>([&](auto v) {
      using Block = tmpl::type_from<decltype(v)>;

      Block b(10UL);
      // if size(vec) <, passes
      CHECK(restricter(b, 5UL));

      b.resize(18UL);
      CHECK(restricter(b, 5UL));

      b.resize(21UL);
      CHECK(restricter(b, 5UL));
    });
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.BlockingPolicy.single_criteria",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    auto custom_criteria = [](auto const&, std::size_t) -> bool {
      return true;
    };
    auto blocker = make_blocking_policy(std::move(custom_criteria));
    static_assert(std::is_same_v<decltype(blocker),
                                   BlockingPolicy<decltype(custom_criteria)>>,
                  "meta failure!");

    // a vector of vectors
    using Block = std::vector<int>;  // needs to have size()
    using Blocks = std::vector<Block>;
    Block first(10UL), second(10UL), third(20UL);
    Blocks b{first, second, third};
    auto&& block_to_emplace = blocker(b, 40UL);
    CHECK(&block_to_emplace == &b[0UL]);

    // chcek if you can push back
    CHECK(b[0UL].back() == 0);
    block_to_emplace.push_back(20);
    CHECK(b[0UL].back() == 20);
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.BlockingPolicy.emplace_if_not_found",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    auto custom_criteria = [](auto const&, std::size_t) -> bool {
      return false;
    };
    auto blocker = make_blocking_policy(std::move(custom_criteria));
    static_assert(std::is_same_v<decltype(blocker),
                                   BlockingPolicy<decltype(custom_criteria)>>,
                  "meta failure!");

    // a vector of vectors
    using Block = std::vector<int>;  // needs to have size()
    using Blocks = std::vector<Block>;
    Block first(10UL), second(10UL), third(20UL);
    Blocks b{first, second, third};

    CHECK(b.size() == 3UL);
    auto&& block_to_emplace = blocker(b, 40UL);
    // after this step, there should have been a fresh block added to the
    // list
    CHECK(b.size() == 4UL);
    CHECK(&block_to_emplace == &b[3UL]);
    // it should also be empty (no modification to the block itself), just
    // default construction.
    CHECK(block_to_emplace.empty());

    // now check if you can push back
    block_to_emplace.push_back(20);
    CHECK(b[3UL].back() == 20);
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.BlockingPolicy_multiple_criteria",
      "[Unit][Simulator][Configuration]") {
    // clang-format on

    // a vector of vectors
    using IntBlock = std::vector<int>;  // needs to have size()
    using CharBlock = std::vector<char>;
    using IntBlocks = std::vector<IntBlock>;
    using CharBlocks = std::vector<CharBlock>;

    auto custom_int_criteria = [](IntBlock const&, std::size_t) -> bool {
      return true;
    };
    auto custom_char_criteria = [](CharBlock const&, std::size_t) -> bool {
      return false;
    };

    auto blocker = make_blocking_policy(std::move(custom_int_criteria),
                                        std::move(custom_char_criteria));

    static_assert(
        std::is_same_v<
            decltype(blocker),
            BlockingPolicy<CompositeCriteria<decltype(custom_int_criteria),
                                             decltype(custom_char_criteria)>>>,
        "meta failure!");

    IntBlock ifirst(10UL), isecond(10UL), ithird(20UL);
    IntBlocks ib{ifirst, isecond, ithird};

    CharBlock cfirst(10UL), csecond(10UL), cthird(20UL);
    CharBlocks cb{cfirst, csecond, cthird};

    auto&& iblock_to_emplace = blocker(ib, 40UL);
    CHECK(&iblock_to_emplace == &ib[0UL]);

    // chcek if you can push back
    CHECK(ib[0UL].back() == 0);
    iblock_to_emplace.push_back(20);
    CHECK(ib[0UL].back() == 20);

    CHECK(cb.size() == 3UL);
    auto&& cblock_to_emplace = blocker(cb, 40UL);
    // after this step, there should have been a fresh block added to the
    // list
    CHECK(cb.size() == 4UL);
    CHECK(&cblock_to_emplace == &cb[3UL]);
    // it should also be empty (no modification to the block itself), just
    // default construction.
    CHECK(cblock_to_emplace.empty());

    // now check if you can push back
    cblock_to_emplace.push_back(20);
    CHECK(cb[3UL].back() == 20);
  }

  struct DefaultConstructible {};

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.RestrictAcrossBlockTypesToPolicy",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using Blocker = RestrictSizeAcrossBlockTypesPolicy;

    // already has make_block embedded into it
    auto blocker = Blocker{Blocker::Criterion{20UL}};

    // anything with a valid size() method
    using FirstBlock = std::vector<int>;
    using SecondBlock = std::list<float>;
    using ThirdBlock = std::deque<DefaultConstructible>;

    tmpl::for_each<tmpl::list<FirstBlock, SecondBlock, ThirdBlock>>(
        [&](auto v) {
          using Block = tmpl::type_from<decltype(v)>;
          using Blocks = std::vector<Block>;

          Block first(10UL), second(15UL), third(30UL);
          Blocks b{first, second, third};

          CHECK(b.size() == 3UL);
          // if size(vec) <, passes
          auto&& block_to_add_stuff_to = blocker(b, 5UL);
          CHECK(&block_to_add_stuff_to == &b[0UL]);

          // add stuff to block so that its just within the capacity
          block_to_add_stuff_to.resize(block_to_add_stuff_to.size() + 5UL);

          auto&& block_to_add_stuff_to_once_more = blocker(b, 5UL);
          CHECK(&block_to_add_stuff_to_once_more == &b[0UL]);

          // add stuff to block so that it exceeds capacity
          block_to_add_stuff_to_once_more.resize(
              block_to_add_stuff_to_once_more.size() + 10UL);

          // gets at 1
          auto&& another_block_to_add_stuff_to = blocker(b, 5UL);
          CHECK(&another_block_to_add_stuff_to == &b[1UL]);
          // add stuff to block so that it exceeds capacity
          another_block_to_add_stuff_to.resize(
              another_block_to_add_stuff_to.size() + 10UL);

          CHECK(b.size() == 3UL);
          // modfies b
          auto&& lastly_block_to_add_stuff_to = blocker(b, 5UL);
          CHECK(b.size() == 4UL);
          CHECK(&lastly_block_to_add_stuff_to == &b[3UL]);
        });
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.LooselyRestrictAcrossBlockTypesToPolicy",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using Blocker = LooselyRestrictSizeAcrossBlockTypesPolicy;

    // already has make_block embedded into it
    auto blocker = Blocker{Blocker::Criterion{20UL}};

    // anything with a valid size() method
    using FirstBlock = std::vector<int>;
    using SecondBlock = std::list<float>;
    using ThirdBlock = std::deque<DefaultConstructible>;

    tmpl::for_each<tmpl::list<FirstBlock, SecondBlock, ThirdBlock>>(
        [&](auto v) {
          using Block = tmpl::type_from<decltype(v)>;
          using Blocks = std::vector<Block>;

          Block first(11UL), second(16UL), third(30UL);
          Blocks b{first, second, third};

          CHECK(b.size() == 3UL);
          // if size(vec) <, passes
          auto&& block_to_add_stuff_to = blocker(b, 5UL);
          CHECK(&block_to_add_stuff_to == &b[0UL]);

          // add stuff to block so that its just within the capacity
          block_to_add_stuff_to.resize(block_to_add_stuff_to.size() + 5UL);

          auto&& block_to_add_stuff_to_once_more = blocker(b, 5UL);
          CHECK(&block_to_add_stuff_to_once_more == &b[0UL]);

          // add stuff to block so that it exceeds capacity
          block_to_add_stuff_to_once_more.resize(
              block_to_add_stuff_to_once_more.size() + 10UL);

          // gets at 1
          auto&& another_block_to_add_stuff_to = blocker(b, 5UL);
          CHECK(&another_block_to_add_stuff_to == &b[1UL]);
          // add stuff to block so that it exceeds capacity
          another_block_to_add_stuff_to.resize(
              another_block_to_add_stuff_to.size() + 10UL);

          CHECK(b.size() == 3UL);
          // modfies b
          auto&& lastly_block_to_add_stuff_to = blocker(b, 5UL);
          CHECK(b.size() == 4UL);
          CHECK(&lastly_block_to_add_stuff_to == &b[3UL]);
        });
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.AlwaysNewAcrossBlockTypesPolicy",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using Blocker = AlwaysNewAcrossBlockTypesPolicy;

    // already has make_block embedded into it
    auto blocker = Blocker{Blocker::Criterion{20UL}};

    // anything with a valid size() method
    using FirstBlock = std::vector<int>;
    using SecondBlock = std::list<float>;
    using ThirdBlock = std::deque<DefaultConstructible>;

    tmpl::for_each<tmpl::list<FirstBlock, SecondBlock, ThirdBlock>>(
        [&](auto v) {
          using Block = tmpl::type_from<decltype(v)>;
          using Blocks = std::vector<Block>;

          Block first(10UL), second(20UL), third(30UL);
          Blocks b{first, second, third};

          CHECK(b.size() == 3UL);
          // modfies b
          auto&& lastly_block_to_add_stuff_to = blocker(b, 5UL);
          CHECK(b.size() == 4UL);
          CHECK(&lastly_block_to_add_stuff_to == &b[3UL]);

          CHECK(b.size() == 4UL);
          // modfies b
          auto&& another_block_to_add_stuff_to = blocker(b, 50UL);
          CHECK(b.size() == 5UL);
          CHECK(&another_block_to_add_stuff_to == &b[4UL]);
        });
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.AlwaysSameAcrossBlockTypesPolicy",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using Blocker = AlwaysSameAcrossBlockTypesPolicy;

    // already has make_block embedded into it
    auto blocker = Blocker{Blocker::Criterion{20UL}};

    // anything with a valid size() method
    using FirstBlock = std::vector<int>;
    using SecondBlock = std::list<float>;
    using ThirdBlock = std::deque<DefaultConstructible>;

    tmpl::for_each<tmpl::list<FirstBlock, SecondBlock, ThirdBlock>>(
        [&](auto v) {
          using Block = tmpl::type_from<decltype(v)>;
          using Blocks = std::vector<Block>;

          Block first(10UL), second(20UL), third(30UL);
          Blocks b{first, second, third};

          CHECK(b.size() == 3UL);
          auto&& lastly_block_to_add_stuff_to = blocker(b, 5UL);
          CHECK(&lastly_block_to_add_stuff_to == &b[0UL]);
          //
          auto&& another_block_to_add_stuff_to = blocker(b, 50UL);
          CHECK(&another_block_to_add_stuff_to == &b[0UL]);
          //
          auto&& yet_another_block_to_add_stuff_to = blocker(b, 50UL);
          CHECK(&yet_another_block_to_add_stuff_to == &b[0UL]);
        });
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.blocking_policy",
      "[Unit][Simulator][Configuration]") {
    // clang-format on

    /// [make_blocking_eg]
    struct FirstSystem {};
    struct SecondSystem {};
    struct ThirdSystem {};
    struct LastSystem {};

    // a system block,  needs to have size() defined
    using FSBlock = std::vector<FirstSystem>;
    using SSBlock = std::vector<SecondSystem>;
    using TSBlock = std::vector<ThirdSystem>;
    using LSBlock = std::vector<LastSystem>;

    // make a policy that allows capacity:
    // <= 20UL for a first block
    // ~<= 50UL for a second block
    // unlimited for a third block
    // only one unit for each last block
    // using namespace ::elastica::configuration;
    auto blocking_policy = ::elastica::configuration::make_blocking_policy(
        ::elastica::configuration::RestrictSize<FSBlock>{20UL},
        ::elastica::configuration::LooselyRestrictSize<SSBlock>{20UL},
        // argument is optional for both of these as they are not used
        ::elastica::configuration::AlwaysSame<TSBlock>{},
        ::elastica::configuration::AlwaysNew<LSBlock>{12UL});
    /// [make_blocking_eg]

    FSBlock fsBlock(20UL);
    SSBlock ssBlock(15UL);
    TSBlock tsBlock(20UL);
    LSBlock lsBlock(40UL);
    CHECK(blocking_policy.criterion_(fsBlock, 10UL) == false);
    CHECK(blocking_policy.criterion_(ssBlock, 20UL) == true);
    CHECK(blocking_policy.criterion_(tsBlock, 20UL) == true);
    CHECK(blocking_policy.criterion_(lsBlock, 10UL) == false);
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.virtualize",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    struct FirstSystem {};
    struct SecondSystem {};
    struct ThirdSystem {};

    using FSBlock = std::vector<FirstSystem>;
    using SSBlock = std::vector<SecondSystem>;
    using TSBlock = std::vector<ThirdSystem>;

    auto blocking_policy = ::elastica::configuration::make_blocking_policy(
        ::elastica::configuration::RestrictSize<FSBlock>{20UL},
        ::elastica::configuration::LooselyRestrictSize<SSBlock>{20UL},
        ::elastica::configuration::AlwaysSame<TSBlock>{});

    auto runtime_polymorphic_blocking =
        ::elastica::configuration::virtualize(blocking_policy);

    // Verify if the current block is included
    tmpl::for_each<tmpl::list<FSBlock, SSBlock, TSBlock>>([&](auto v) {
      using Block = tmpl::type_from<decltype(v)>;
      using Blocks = std::vector<Block>;

      Block first(10UL), second(20UL), third(30UL);
      Blocks b{first, second, third};

      CHECK(b.size() == 3UL);
      CHECK(runtime_polymorphic_blocking->get_iterator_to_block(b, 13UL) ==
            blocking_policy.get_iterator_to_block(b, 13UL));
      CHECK(runtime_polymorphic_blocking->get_iterator_to_block(b, 20UL) ==
            blocking_policy.get_iterator_to_block(b, 20UL));
      CHECK(runtime_polymorphic_blocking->get_iterator_to_block(b, 20UL) ==
            blocking_policy.get_iterator_to_block(b, 20UL));
    });
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.runtime_polymorphism",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    struct FirstSystem {};
    struct SecondSystem {};
    struct ThirdSystem {};

    using FSBlock = std::vector<FirstSystem>;
    using SSBlock = std::vector<SecondSystem>;
    using TSBlock = std::vector<ThirdSystem>;

    // Always new policies here so that the failures inside check below are
    // immediate and obvious
    auto blocking_policy = ::elastica::configuration::make_blocking_policy(
        ::elastica::configuration::AlwaysNew<FSBlock>{20UL},
        ::elastica::configuration::AlwaysNew<SSBlock>{20UL},
        ::elastica::configuration::AlwaysNew<TSBlock>{20UL});

    // already tested
    auto runtime_polymorphic_blocking =
        ::elastica::configuration::virtualize(blocking_policy);

    // The size parameter cannot be small, else it tries to create a new block
    // everytime
    auto const another_policy = ::elastica::configuration::make_blocking_policy(
        ::elastica::configuration::RestrictSizeAcrossBlockTypes{30UL});

    auto another_runtime =
        ::elastica::configuration::virtualize(another_policy);
    ::elastica::ignore_unused(another_runtime);

    runtime_polymorphic_blocking = ::elastica::configuration::virtualize<
        typename decltype(runtime_polymorphic_blocking)::element_type::
            registrars>(another_policy);

    tmpl::for_each<tmpl::list<FSBlock, SSBlock, TSBlock>>([&](auto v) {
      using Block = tmpl::type_from<decltype(v)>;
      using Blocks = std::vector<Block>;

      Block first(10UL), second(20UL), third(30UL);
      Blocks b{first, second, third};

      CHECK(b.size() == 3UL);

      CHECK(runtime_polymorphic_blocking->get_iterator_to_block(b, 13UL) ==
            another_policy.get_iterator_to_block(b, 13UL));
      CHECK(runtime_polymorphic_blocking->get_iterator_to_block(b, 13UL) !=
            blocking_policy.get_iterator_to_block(b, 13UL));

      CHECK(runtime_polymorphic_blocking->get_iterator_to_block(b, 20UL) ==
            another_policy.get_iterator_to_block(b, 20UL));
      CHECK(runtime_polymorphic_blocking->get_iterator_to_block(b, 20UL) !=
            blocking_policy.get_iterator_to_block(b, 20UL));

      // 25 < 30, the size of the other policy
      CHECK(runtime_polymorphic_blocking->get_iterator_to_block(b, 25UL) ==
            another_policy.get_iterator_to_block(b, 25UL));
      CHECK(runtime_polymorphic_blocking->get_iterator_to_block(b, 25UL) !=
            blocking_policy.get_iterator_to_block(b, 25UL));
    });
  }

  struct OptionType {
    using element_type = ::elastica::configuration::detail::BlockingPolicyBase<
        ::elastica::configuration::detail::BlockingPolicyRegistrars>;
    using type = std::unique_ptr<element_type>;
    static constexpr Options::String help = {"The type of BlockingPolicy"};
    // static std::unique_ptr<OptionTest> default_value() noexcept;
  };

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.Options",
      "[Unit][Simulator][Configuration]") {
    // clang-format on

    auto get_name = [](auto* result) {
      return call_with_dynamic_type<
          std::string, typename OptionType::element_type::creatable_classes>(
          result, [](auto* const blocking_policy) {
            return pretty_type::name<decltype(blocking_policy->criterion_)>();
          });
    };

    auto check_parsing = [=](std::string const& eman) {
      Options::Parser<tmpl::list<OptionType>> opts("");
      std::stringstream ss;
      // clang-format off
      ss << "OptionType:\n"
            "  " << eman << ":\n"
            "    CriterionParameter:\n"
            "      Size: 20\n";
      // clang-format on
      opts.parse(ss.str());
      auto a = opts.get<OptionType>();
      CHECK(get_name(a.get()) == eman);
    };

    check_parsing("RestrictSizeAcrossBlockTypes");
    check_parsing("LooselyRestrictSizeAcrossBlockTypes");
    check_parsing("AlwaysSameAcrossBlockTypes");
    check_parsing("AlwaysNewAcrossBlockTypes");
  }

}  // namespace
