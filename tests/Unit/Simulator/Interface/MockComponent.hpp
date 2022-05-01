//
// Created by Tejaswin Parthasarathy on 6/21/21.
//

#pragma once

#include "Utilities/MakeFromTuple.hpp"
#include "Utilities/TMPL.hpp"

// useful to test a bare-bones simulator
namespace mock {

  struct EmptyConfiguration {
    EmptyConfiguration() : counter_(0UL){};
    std::size_t counter() const { return counter_; }
    std::size_t counter_;
  };

  template <typename T, typename S>
  struct EmptyModule {
    using TimeType = typename T::TimeType;
    void finalize(TimeType) {}

    struct UpdateStep {
      UpdateStep(S*, T const& traits) : counter{traits.counter()} {};
      explicit UpdateStep(std::tuple<S*, T const&>&& args)
          : UpdateStep(cpp17::make_from_tuple<UpdateStep>(std::move(args))) {}

      void operator()(TimeType) const { ++counter; }
      mutable std::size_t counter;
    };

    struct FirstUpdateStep {
      using type = UpdateStep;
    };
    struct SecondUpdateStep {
      using type = UpdateStep;
    };
    struct ThirdUpdateStep {
      using type = UpdateStep;
    };

    struct Batcher {
      struct MockBatcher {
        MockBatcher(T const&){};
      };
      using type = MockBatcher;
    };

    struct UpdateStepWrapper{
      using type = UpdateStep;
    };

    using UpdateSteps = tmpl::list<UpdateStepWrapper>;
    using PostKinematicUpdateSteps = tmpl::list<FirstUpdateStep>;
    using PreDynamicUpdateSteps = tmpl::list<SecondUpdateStep>;
    using PostDynamicUpdateSteps = tmpl::list<ThirdUpdateStep>;
    using BatchingAlgorithm = tmpl::list<Batcher>;
  };

}  // namespace mock
