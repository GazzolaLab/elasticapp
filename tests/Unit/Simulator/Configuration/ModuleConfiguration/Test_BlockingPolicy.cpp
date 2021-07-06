//
// Created by Tejaswin Parthasarathy on 6/16/21.
//
#include <deque>
#include <list>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Configuration/ModuleConfiguration/BlockingPolicy.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {

  using namespace elastica::configuration;

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.RestrictSizeOf",
      "[Unit][Simulator][Configuration]") {
    // clang-format on

    // anything with a valid size
    using Block = std::vector<int>;
    RestrictSizeOf<Block> restricter{20UL};

    Block b(10UL);
    // if size(vec) <, passes
    CHECK(restricter(b, 5UL));

    b.resize(18UL);
    CHECK(not restricter(b, 5UL));
  }

  // clang-format off
  ELASTICA_TEST_CASE(
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.LooselyRestrictSizeOf",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    // anything with a valid size
    using Block = std::vector<int>;
    LooselyRestrictSizeOf<Block> restricter{20UL};

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
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.RestrictSizeOfAcrossBlockTypes",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using Restricter = AcrossBlockTypes<RestrictSizeOf>;

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
      "Unit.Simulator.Configuration.ModuleConfiguration.Blocking.LooselyRestrictSizeOfAcrossBlockTypes",
      "[Unit][Simulator][Configuration]") {
    // clang-format on
    using Restricter = AcrossBlockTypes<LooselyRestrictSizeOf>;

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
    static_assert(cpp17::is_same_v<decltype(blocker),
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
    static_assert(cpp17::is_same_v<decltype(blocker),
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
        cpp17::is_same_v<
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
    auto blocker = Blocker{20UL};

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
    auto blocker = Blocker{20UL};

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
    auto blocker = Blocker{20UL};

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
    auto blocker = Blocker{20UL};

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
        ::elastica::configuration::RestrictSizeOf<FSBlock>{20UL},
        ::elastica::configuration::LooselyRestrictSizeOf<SSBlock>{20UL},
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

}  // namespace
