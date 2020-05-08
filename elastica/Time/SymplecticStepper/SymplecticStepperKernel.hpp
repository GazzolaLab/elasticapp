#pragma once
#include <type_traits>

#include "../StepperTags.hpp"
#include "SymplecticStepperTags.hpp"

namespace timestepper {
namespace symplectic {

template <class SymplecticAlgorithm,
          symplectic::MirroredAbout M = SymplecticAlgorithm::mirrored_about()>
struct SymplecticStepperKernel;

template <class SymplecticAlgorithm>
struct SymplecticStepperKernel<SymplecticAlgorithm,
                               MirroredAbout::position_step> {
public:
  using StepperCategoryTag = SymplecticTag;
  // Obly
  static constexpr std::size_t n_steps() { return 2 * n_stages() + 1UL; };
  static constexpr std::size_t nfeval_per_step() {
    // TODO : Confirm if the formula is right
    return 2 * n_stages() + 1UL;
  };

  static constexpr std::size_t n_double_stages() { return 2 * n_stages(); };
  static constexpr std::size_t n_stages() {
    return SymplecticAlgorithm::n_stages();
  };
  // constexpr std::size_t Order = SymplecticAlgorithm::Order;

  // clang-format off
  // Overall sequnce of steps is K0, D0, K1, D1, K2, D2...K_(order-1), D_(order-1), ||| K_(order) ||| , D_(order-1), K_(order - 1), Dn-2, .... K1, D0, K0
  // Total of 4 * order + 1 steps
  // Extend to these steps : K0, D0, K1, D1, K2, D2...K_(order-1), D_(order-1), ||| K_(order) ||| , D_(order-1), K_(order - 1), Dn-2, .... K1, D0, K0, NULLSTEP
  // Total of 4 * order + 2 steps
  // Now we just do it in (K0, D0), (K1, D1),...... (K_(order-1), D_(order-1)),  (K_(order), D_(order-1)), (K_(order -1), D_(order - 2)), ...... (K1, D0), (K0, NULLSTEP)
  // Till (K_(order-1), D_(order-1)) there are order pairs having (2 * order) steps : outside iteration from 0 to Order-1
  // From that point onwards there are order + 1 pairs  having (2 * (order + 1)) steps : outside iteration from Order to 2 * Order

  // clang-format on

  // This does the part till : So K0, K1, K2, ... K_order-1, K_order
  // Currentstage [0, Stage]
  // template <std::size_t CurrentStage, typename  std::enable_if<(CurrentStage
  // <= Stage), int>::type> template <std::size_t CurrentStage, typename =
  // std::enable_if_t<(CurrentStage <= Stage)> >
  template <
      std::size_t CurrentStage,
      typename std::enable_if<(CurrentStage <= n_stages()), int>::type = 0>
  inline void kinematic_step(float k) {
    algorithm.template kinematic_step<CurrentStage>(k);
  };

  // This does the part till : So K_{Stage-1}, ...     K1,
  // .............................Stage + 1, ... 2*Stage-1
  // CurrentStage : (Stage, 2 * Stage)
  // template <std::size_t CurrentStage, typename std::enable_if< ((Stage <
  // CurrentStage) && (CurrentStage <= n_double_stages())), int>::type=0>
  template <std::size_t CurrentStage,
            typename std::enable_if<((n_stages() < CurrentStage) &&
                                     (CurrentStage < n_double_stages())),
                                    int>::type = 0>
  inline void kinematic_step(float k) {
    algorithm.template kinematic_step<n_stages() - CurrentStage % n_stages()>(
        k);
  };

  // CurrentStage : 2 * Stage
  template <
      std::size_t CurrentStage,
      typename std::enable_if<CurrentStage == n_double_stages(), int>::type = 0>
  inline void kinematic_step(float k) {
    algorithm.template kinematic_step<0UL>(k);
  };

  // This does the part till : So D0, D1, D2, ... D_order-1
  // Currentstage [0, Stage)
  // template <std::size_t CurrentStage, typename std::enable_if< (CurrentStage
  // < Stage) >::type=0> template <std::size_t CurrentStage, typename =
  // std::enable_if_t< (CurrentStage < Stage) >>
  template <std::size_t CurrentStage,
            typename std::enable_if<(CurrentStage < n_stages()), int>::type = 0>
  inline void dynamic_step(float k) {
    algorithm.template dynamic_step<CurrentStage>(k);
  };

  // This does the part till : So D_order-1, ...  D2,            D1,      D0,
  // .............................Stage , ... 2*Stage-3 , 2*Stage - 2,  2 *
  // Stage - 1 Currentstage [Stage, 2 * Stage) template <std::size_t
  // CurrentStage, typename std::enable_if< ((Stage <= CurrentStage) && (
  // CurrentStage < n_double_stages())) >::type=0> template <std::size_t
  // CurrentStage, typename = std::enable_if_t< ((Stage <= CurrentStage) && (
  // CurrentStage < n_double_stages())) >>
  template <std::size_t CurrentStage,
            typename std::enable_if<((n_stages() <= CurrentStage) &&
                                     (CurrentStage < n_double_stages())),
                                    int>::type = 0>
  inline void dynamic_step(float k) {
    algorithm
        .template dynamic_step<n_stages() - CurrentStage % n_stages() - 1UL>(k);
  };

  // Null op
  // Currentstage : 2 * Stage
  // template <std::size_t CurrentStage, typename std::enable_if< (CurrentStage
  // == n_double_stages()) >::type=0>
  template <
      std::size_t CurrentStage,
      typename std::enable_if<CurrentStage == n_double_stages(), int>::type = 0>
  inline void dynamic_step(float /*k*/){};

  // TODO Testing logic. Sigma of all prefactors should sum up to one.
  // TODO Replace double by TimeType
  // Copy paste the kinematic_step logic here
  template <
      std::size_t CurrentStage,
      typename std::enable_if<(CurrentStage <= n_stages()), int>::type = 0>
  inline double kinematic_prefactor() {
    return algorithm.template kinematic_prefactor<CurrentStage>();
  };

  template <std::size_t CurrentStage,
            typename std::enable_if<((n_stages() < CurrentStage) &&
                                     (CurrentStage < n_double_stages())),
                                    int>::type = 0>
  inline double kinematic_prefactor() {
    return algorithm
        .template kinematic_prefactor<n_stages() - CurrentStage % n_stages()>();
  };

  template <
      std::size_t CurrentStage,
      typename std::enable_if<CurrentStage == n_double_stages(), int>::type = 0>
  inline double kinematic_prefactor() {
    return algorithm.template kinematic_prefactor<0UL>();
  };

  SymplecticAlgorithm algorithm;
};

template <class SymplecticAlgorithm>
struct SymplecticStepperKernel<SymplecticAlgorithm,
                               MirroredAbout::velocity_step> {
public:
  using StepperCategoryTag = SymplecticTag;
  // This is the only thing that should be exposed to be used by the
  // timealgorithm class
  static constexpr std::size_t n_steps() { return 2 * n_stages(); };
  static constexpr std::size_t nfeval_per_step() {
    return 2 * n_stages() - 1UL;
  };
  static constexpr std::size_t n_double_stages() { return 2 * n_stages(); };
  static constexpr std::size_t n_stages() {
    return SymplecticAlgorithm::n_stages();
  };
  // static constexpr std::size_t Order = SymplecticAlgorithm::Order;

  // clang-format off
    // Overall sequnce of steps is K0, D0, K1, D1, K2, D2...K_(order-1), ||| D_(order-1) |||,  K_(order - 1), Dn-2, .... K1, D0, K0
    // Total of 4 * order - 1 steps
    // Extend to these steps : K0, D0, K1, D1, K2, D2...K_(order-1), ||| D_(order-1), |||  K_(order - 1), Dn-2, .... K1, D0, K0, NULLSTEP
    // Total of 4 * order
    // Now we just do it in (K0, D0), (K1, D1),...... (K_(order-1), D_(order-1)),  (K_(order -1), D_(order - 2)), ...... (K1, D0), (K0, NULLSTEP)
    // Till (K_(order-1), D_(order-1)) there are order pairs having (2 * order) steps : outside iteration from 0 to Order-1
    // From that point onwards there are order  having (2 * order) steps : outside iteration from Order to 2 * Order - 1
  // clang-format on

  // This does the part till : So K0, K1, K2, ... K_order-1
  // Currentstage [0, Stage)
  // template <std::size_t CurrentStage, typename  std::enable_if<(CurrentStage
  // <= Stage), int>::type> template <std::size_t CurrentStage, typename =
  // std::enable_if_t<(CurrentStage <= Stage)> >
  template <std::size_t CurrentStage, class System,
            typename std::enable_if<(CurrentStage < n_stages()), int>::type = 0>
  inline void kinematic_step(System &k, double time_v, double dt) {
    algorithm.template kinematic_step<CurrentStage>(k, time_v, dt);
  };

  // This does the part till : So K_{Stage-1}, ...     K0,
  // .............................Stage + 1, ... 2*Stage-1
  // CurrentStage : [Stage, 2 * Stage)
  // template <std::size_t CurrentStage, typename std::enable_if< ((Stage <
  // CurrentStage) && (CurrentStage <= DoubleStage)), int>::type=0>
  template <std::size_t CurrentStage, class System,
            typename std::enable_if<((n_stages() <= CurrentStage) &&
                                     (CurrentStage < n_double_stages())),
                                    int>::type = 0>
  inline void kinematic_step(System &k, double time_v, double dt) {
    algorithm
        .template kinematic_step<n_stages() - CurrentStage % n_stages() - 1UL>(
            k, time_v, dt);
  };

  // This does the part till : So D0, D1, D2, ... D_order-1
  // Currentstage [0, Stage)
  // template <std::size_t CurrentStage, typename std::enable_if< (CurrentStage
  // < Stage) >::type=0> template <std::size_t CurrentStage, typename =
  // std::enable_if_t< (CurrentStage < Stage) >>
  template <std::size_t CurrentStage, class System,
            typename std::enable_if<(CurrentStage < n_stages()), int>::type = 0>
  inline void dynamic_step(System &k, double time_v, double dt) {
    algorithm.template dynamic_step<CurrentStage>(k, time_v, dt);
  };

  // This does the part till : So D_order-1, ...  D2,            D1,      D0,
  // .............................Stage , ... 2*Stage-3 , 2*Stage - 2,  2 *
  // Stage - 1 Currentstage [Stage, 2 * Stage) template <std::size_t
  // CurrentStage, typename std::enable_if< ((Stage <= CurrentStage) && (
  // CurrentStage < DoubleStage)) >::type=0> template <std::size_t CurrentStage,
  // typename = std::enable_if_t< ((Stage <= CurrentStage) && ( CurrentStage <
  // DoubleStage)) >>
  template <
      std::size_t CurrentStage, class System,
      typename std::enable_if<((n_stages() <= CurrentStage) &&
                               ((CurrentStage + 1UL) < n_double_stages())),
                              int>::type = 0>
  inline void dynamic_step(System &k, double time_v, double dt) {
    algorithm
        .template dynamic_step<n_stages() - CurrentStage % n_stages() - 2UL>(
            k, time_v, dt);
  };

  // Null op
  // Currentstage : 2 * Stage
  // template <std::size_t CurrentStage, typename std::enable_if< (CurrentStage
  // == DoubleStage) >::type=0>
  template <std::size_t CurrentStage, class System,
            typename std::enable_if<(CurrentStage + 1UL) == n_double_stages(),
                                    int>::type = 0>
  inline void dynamic_step(System & /*k*/, double time_v, double dt){};

  template <std::size_t CurrentStage,
            typename std::enable_if<(CurrentStage < n_stages()), int>::type = 0>
  inline double kinematic_prefactor() {
    return algorithm.template kinematic_prefactor<CurrentStage>();
  };

  template <std::size_t CurrentStage,
            typename std::enable_if<((n_stages() <= CurrentStage) &&
                                     (CurrentStage < n_double_stages())),
                                    int>::type = 0>
  inline double kinematic_prefactor() {
    return algorithm.template kinematic_prefactor<
        n_stages() - CurrentStage % n_stages() - 1UL>();
  };

  SymplecticAlgorithm algorithm;
};
} // namespace symplectic

} // namespace timestepper
