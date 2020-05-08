#pragma once

#include <algorithm>
#include <tuple>
#include <type_traits>
#include <vector>

#include "elastica/Systems/SystemBase.hpp"
#include "elastica/Utilities/Logging.hpp"
#include "elastica/Utilities/Logging/Logger.hpp"
#include "elastica/Utilities/Singleton.hpp"
#include "elastica/Utilities/TypeTraits.hpp"

namespace elastica {
  namespace interface {
    namespace detail {
      // Doesn't short-circuit by intention
      // If you pass a parameter pack <A,B,C,D,E> does compare of
      // A,  B,  C, (D, E)
      // A,  B, (C, (D, E))
      // A, (B, (C, (D, E)))
      // finally compares all
      template <template <typename, typename> class Conditional, class Head,
                class... Tail>
      struct PairwiseTypeConjunction {
        using type = Conditional<
            Head, typename PairwiseTypeConjunction<Conditional, Tail...>::type>;
      };

      template <template <typename, typename> class Conditional, class Head>
      struct PairwiseTypeConjunction<Conditional, Head> {
        using type = Head;
      };
    }  // namespace detail

    template <template <typename, typename> class Conditional, class Head,
              class... Tail>
    using pairwise_type_conjunction_t =
        typename detail::PairwiseTypeConjunction<Conditional, Head,
                                                 Tail...>::type;

    // TODO : Mark it as unused for optimization
    template <typename SimTraits>
    struct DefaultInterface {
      using SizeType = typename SimTraits::SizeType;
      using IndexType = typename SimTraits::IndexType;
      using TimeType = typename SimTraits::TimeType;
      template <typename T>
      using SystemInterfaceType =
          typename SimTraits::template SystemInterfaceType<T>;

      DefaultInterface() = default;
      ~DefaultInterface() = default;
      // Anything default goes here
      inline void constrain_primitive_values(TimeType /*time*/){};
      inline void constrain_primitive_rates(TimeType /*time*/){};
    };

    // DONE : Check for mixins to simulator class should always be default
    // constructible (not trivial, but default)
    // TODO : Make it Singleton/Non-copyable?
    template <typename SimTraits,
              template <typename /*Simulator*/, typename /*SimTraits*/>
              class... Bases>
    class Simulator
        : public DefaultInterface<SimTraits>,
          public Singleton<Simulator<SimTraits, Bases...>, logging::Logger>,
          public Bases<Simulator<SimTraits, Bases...>, SimTraits>... {
     public:
      static_assert(
          cpp17::conjunction_v<
              std::is_default_constructible<Bases<int, SimTraits>>...>,
          "Simulation options must have a default/empty constructor!");

      // Make all the bases friends by default
      // Awesome trick to do it, very clever
      // Idea is to construct temporarily a tuple with all base types
      // and then unpack it one by one and make it a friend
      // https://stackoverflow.com/a/31514214

#define REPEAT_2(M, N) M(N) M(N + 1)
#define REPEAT_4(M, N) REPEAT_2(M, N) REPEAT_2(M, N + 2)
#define REPEAT_8(M, N) REPEAT_4(M, N) REPEAT_4(M, N + 4)
#define FRIEND(N)                                                  \
  friend std::tuple_element_t<                                     \
      std::min(static_cast<std::size_t>(N + 1), sizeof...(Bases)), \
      std::tuple<void, Bases<Simulator<SimTraits, Bases...>, SimTraits>...>>;

      REPEAT_8(FRIEND, 0)

#undef FRIEND
#undef REPEAT_8
#undef REPEAT_4
#undef REPEAT_2

// Same WTF trick as in gravitas
// https://stackoverflow.com/a/21175219

// DONE : Putting this inside the simluator class makes more sense given
// thats the ONLY use for these compile-time data-structures
#define xxx(x, y) x##y
#define CONCAT(x, y) xxx(x, y)

// Choose which one has the function
#define STAMP_OUT_SIMULATION_METHOD_CHOOSERS(NAME, METHOD) \
  template <class A, class B, typename = void>             \
  struct NAME {                                            \
    using type = A;                                        \
  };                                                       \
                                                           \
  template <class A, class B>                              \
  struct NAME<A, B, cpp17::void_t<decltype(&B::METHOD)>> { \
    using type = B;                                        \
  };                                                       \
  template <class A, class B>                              \
  using CONCAT(NAME, _t) = typename NAME<A, B>::type;      \
                                                           \
  template <typename Head, class... Rest>                  \
  using CONCAT(NAME, Chooser) =                            \
      pairwise_type_conjunction_t<CONCAT(NAME, _t), Head, Rest...>;

      STAMP_OUT_SIMULATION_METHOD_CHOOSERS(HasConstraintRates,
                                           constrain_primitive_rates)
      STAMP_OUT_SIMULATION_METHOD_CHOOSERS(HasConstraintValues,
                                           constrain_primitive_values)
#undef STAMP_OUT_SIMULATION_METHOD_CHOOSERS
#undef CONCAT
#undef xxx

      // Choose overloaded functions based on constructed sim
      using HasConstraintRatesChooser<
          DefaultInterface<SimTraits>,
          Bases<Simulator<SimTraits, Bases...>,
                SimTraits>...>::constrain_primitive_rates;
      using HasConstraintValuesChooser<
          DefaultInterface<SimTraits>,
          Bases<Simulator<SimTraits, Bases...>,
                SimTraits>...>::constrain_primitive_values;

      //      USE_SIMULATOR_TRAITS(SimTraits)
      using SizeType = typename SimTraits::SizeType;
      using IndexType = typename SimTraits::IndexType;
      using TimeType = typename SimTraits::TimeType;
      template <typename T>
      using SystemInterfaceType =
          typename SimTraits::template SystemInterfaceType<T>;

      using SystemType = SystemInterfaceType<::elastica::SystemBase>;
      using SystemID = ::elastica::SystemBase*;

      // TODO : Make private, friends can still access!
      template <typename System>
      IndexType check(const SystemInterfaceType<System>& system) {
        auto it = std::find(systems_.begin(), systems_.end(), system.get());
        if (it == systems_.end())
          throw std::range_error("System is invalid");
        return std::distance(systems_.begin(), it);
      }

      // TODO : Make private, friends can still access!
      SizeType dofs(IndexType sys_idx) {
        if (sys_idx >= systems_.size())
          throw std::range_error("System is invalid");
        // return systems_[sys_idx]->dofs();
        return 6UL;
      }

      // TODO : Make private, friends can still access!
      SystemID operator[](IndexType idx) { return systems_[idx]; }

      // TODO : Make private
      template <
          class System,
          typename std::enable_if<
              (std::is_convertible<System*, SystemID>::value), int>::type = 0>
      inline void push_back_impl(System* system_ptr) {
        systems_.push_back(static_cast<SystemID>(system_ptr));
      }

      template <class System,
                typename std::enable_if<(std::is_same<System*, double*>::value),
                                        int>::type = 0>
      inline void push_back_impl(System* system_ptr) {
        double_systems_.push_back(system_ptr);
      }

      // Vector like interface here, mixin?
      template <class System>
      inline void push_back(const SystemInterfaceType<System>& system) {
        // TODO Push back dispatch to different vectors based on admissible
        // types
        static_assert(std::is_base_of<SystemBase, System>::value ||
                          std::is_same<double, System>::value,
                      "Cannot add to simulator now boi");
        push_back_impl(system.get());
      }
      auto begin() { return systems_.begin(); }
      auto end() { return systems_.end(); }

      void finalize(TimeType time_v = 0.0) {
        // Travel up the hierarchy and call finalize on all
        using expander = int[];
        (void)expander{
            0,
            ((void)Bases<Simulator<SimTraits, Bases...>, SimTraits>::finalize(
                 time_v),
             0)...};
      }
      ~Simulator(){
          LOG_INFO_SECTION(log) {
              log << "Successfully destroyed the simulation instance";
            }
      };

     private:
      //      class Simulator : public DefaultInterface<SimTraits>,
      //                        private Singleton<Simulator<SimTraits,
      //                        Bases...>, logging::Logger>, public
      //                        Bases<Simulator<SimTraits, Bases...>,
      //                        SimTraits>... {
      Simulator()
          : DefaultInterface<SimTraits>(),
            Singleton<Simulator<SimTraits, Bases...>, logging::Logger>(),
            Bases<Simulator<SimTraits, Bases...>, SimTraits>()... {
          LOG_INFO_SECTION(log) {
            log << "Successfully initialized the simulation instance";
          }
      }

      BEFRIEND_SINGLETON

      template <template <typename /*Simulator*/, typename /*SimTraits*/>
                class... AddOns,
                typename SimulationOptions_>
      friend auto make_simulator(SimulationOptions_);

      /*
      int get(std::size_t idx){
      return d[idx];
    }
      */

      std::vector<SystemID> systems_;
      // TODO : Delete, purely for testing purposes
      std::vector<double*> double_systems_;
    };  // namespace interface

  }  // namespace interface
}  // namespace elastica
