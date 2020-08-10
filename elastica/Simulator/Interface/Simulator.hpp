#pragma once

#include <algorithm>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "elastica/Simulator/Domain/Domain.hpp"
#include "elastica/Systems/SystemBase.hpp"
#include "elastica/Utilities/Logging.hpp"
#include "elastica/Utilities/Logging/Logger.hpp"
#include "elastica/Utilities/Singleton.hpp"
#include "elastica/Utilities/TypeTraits.hpp"
#include "elastica/Utilities/ValueTraits/IsInf.hpp"

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
      using TimeType = typename SimTraits::TimeType;
      template <typename T>
      using SystemInterfaceType =
          typename SimTraits::template SystemInterfaceType<T>;

      DefaultInterface() = default;
      ~DefaultInterface() = default;
      // Anything default goes here, redundant as hell
      __unused inline void constrain_primitive_values(TimeType /*time*/){};
      inline void constrain_primitive_rates(TimeType /*time*/){};
      inline void resolve_effects(TimeType /*time*/){};
    };

    // DONE : Check for mixins to simulator class should always be default
    // constructible (not trivial, but default)
    // TODO : Make it Singleton/Non-copyable?
    template <typename SimTraits,
              template <typename /*Simulator*/, typename /*SimTraits*/>
              class... Bases>
    class Simulator
        : public Singleton<Simulator<SimTraits, Bases...>, logging::Logger>,
          public DefaultInterface<SimTraits>,
          public Bases<Simulator<SimTraits, Bases...>, SimTraits>... {
     public:
      static_assert(
          cpp17::conjunction_v<
              std::is_default_constructible<Bases<int, SimTraits>>...>,
          "Simulation enhancements must have a default/empty constructor!");

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
      STAMP_OUT_SIMULATION_METHOD_CHOOSERS(HasResolveEffects, resolve_effects)
#undef STAMP_OUT_SIMULATION_METHOD_CHOOSERS
#undef CONCAT
#undef xxx

      // TODO : Use tmpl::find_if to select overload of rates and values
      //      // Choose overloaded functions based on constructed sim
      //      using EnhancementsList =
      //          tmpl::list<Bases<Simulator<SimTraits, Bases...>,
      //          SimTraits>...>;
      //      using EnhancementInterfaceList =
      //          tmpl::push_front<DefaultInterface<SimTraits>,
      //          EnhancementsList>;
      //
      //
      using HasConstraintRatesChooser<
          DefaultInterface<SimTraits>,
          Bases<Simulator<SimTraits, Bases...>,
                SimTraits>...>::constrain_primitive_rates;
      using HasConstraintValuesChooser<
          DefaultInterface<SimTraits>,
          Bases<Simulator<SimTraits, Bases...>,
                SimTraits>...>::constrain_primitive_values;
      using HasResolveEffectsChooser<
          DefaultInterface<SimTraits>,
          Bases<Simulator<SimTraits, Bases...>, SimTraits>...>::resolve_effects;

      using SizeType = std::size_t;
      using IndexType = std::size_t;
      using TimeType = typename SimTraits::TimeType;

      //      template <typename T>
      //      using AddSystemInterfaceType =
      //          typename SimTraits::template AddSystemInterfaceType<T>;

      //      template <typename T>
      //      using SystemInterfaceType = typename
      //      AddSystemInterfaceType<T>::type;

      //      template <typename ID>
      //      using AddSystemCollectionStorage =
      //          typename SimTraits::template AddSystemStorageType<ID>;
      //
      template <typename SystemCollection>
      using AddSystemCollectionStorageType =
          typename SimTraits::template AddSystemStorageType<SystemCollection>;

      // using SystemIDTypes =
      //    tmpl::transform<AdmissibleSystems, std::add_pointer<tmpl::_1>>;

      // Typelist of Admissible systems, say <RigidBodyBase, SystemBase>
      using AdmissibleSystemCollections =
          typename SimTraits::AdmissibleSystemCollections;

      // Typelist of interfaces of these Admissible systems,
      // say < std::shared_ptr<RigidBodyBase>, std::shared_ptr<SystemBase> >
      //      using SystemInterfaceTypes =
      //          tmpl::transform<AdmissibleSystems,
      //          AddSystemInterfaceType<tmpl::_1>>;

      // Typelist of collection of pointers of these Admissible systems,
      // say < std::vector<RigidBodyBase*>, std::vector<SystemBase*> >
      //      using SystemIDCollectionTypes =
      //          tmpl::transform<AdmissibleSystems,
      //          SystemIDCollectionType<tmpl::_1>>;

      //      using SystemCollectionTypes =
      //          tmpl::transform<AdmissibleSystems,
      //          AddSystemStorageType<tmpl::_1>>;
      // Tuple of collection of pointers of these Admissible systems,
      // say std::tuple< std::vector<RigidBodyBase*>, std::vector<SystemBase*> >
      using SystemCollectionStorage = tmpl::as_tuple<tmpl::transform<
          AdmissibleSystemCollections,
          tmpl::bind<AddSystemCollectionStorageType, tmpl::_1>>>;
      //      using SystemIDCollection =
      //          std::tuple<std::vector<SystemBase*>,
      //                     std::vector<::elastica::rigidbody::Sphere*>>;

      template <std::size_t N>
      using BoundaryTypeAlong =
          typename SimTraits::template BoundaryTypeAlong<N>;
      using DomainType = typename SimTraits::DomainType;

      // Since this is an internal function, we give a reference and not a const
      // reference So things like environments are allowed to change properties
      // of the collection containers (they can add arbitrary forces to the
      // bodies, for example) its up to the calling scope to determine whether a
      // const ref or a non-const ref is needed
      inline constexpr auto get_system_collection_storage() noexcept
          -> SystemCollectionStorage& {
        return system_collection_containers_;
      }

      inline constexpr auto get_system_collection_storage() const noexcept
          -> SystemCollectionStorage const& {
        return system_collection_containers_;
      }

      // TODO : Private (outside world has no business accessing the vector of
      //        collections,
      // Do not delcare rvalue gets
      template <typename SystemCollection>
      inline constexpr auto get_system_collection_container() noexcept
          -> AddSystemCollectionStorageType<SystemCollection>& {
        return std::get<AddSystemCollectionStorageType<SystemCollection>>(
            system_collection_containers_);
      }

      template <typename SystemCollection>
      inline constexpr auto get_system_collection_container() const noexcept
          -> AddSystemCollectionStorageType<SystemCollection> const& {
        return std::get<AddSystemCollectionStorageType<SystemCollection>>(
            system_collection_containers_);
      }

      // TODO : Private
      template <SizeType SystemCollectionIdx>
      inline constexpr auto get_system_collection_container() noexcept
          -> std::tuple_element_t<SystemCollectionIdx,
                                  SystemCollectionStorage>& {
        static_assert(SystemCollectionIdx < get_number_of_system_collections(),
                      "Requested > present");
        return std::get<SystemCollectionIdx>(system_collection_containers_);
      }

      //      using SystemType = SystemInterfaceType<::elastica::SystemBase>;
      //      using SystemID = ::elastica::SystemBase*;

      // TODO : Make private, friends can still access!
      //      template <typename System>
      //      IndexType check(const SystemInterfaceType<System>& system) {
      //        auto it = std::find(systems_.begin(), systems_.end(),
      //        system.get()); if (it == systems_.end())
      //          throw std::range_error("System is invalid");
      //        return std::distance(systems_.begin(), it);
      //      }

      template <typename SystemCollection>
      inline auto begin() {
        return get_system_collection_container<SystemCollection>().begin();
      }
      template <typename SystemCollection>
      inline auto end() {
        return get_system_collection_container<SystemCollection>().end();
      }

      template <typename SystemCollection>
      IndexType check(const SystemCollection& system) {
        const auto _begin_ = begin<SystemCollection>();
        const auto _end_ = end<SystemCollection>();

        auto it = std::find(_begin_, _end_, system);

        if (it == _end_)
          throw std::range_error("System is invalid");

        return std::distance(_begin_, it);
      }

      // TODO : Make private, friends can still access!
      SizeType dofs(IndexType /*sys_idx*/) {
        //        if (sys_idx >= systems_.size())
        //          throw std::range_error("System is invalid");
        // return systems_[sys_idx]->dofs();
        return 6UL;
      }

      // TODO : Make private, friends can still access!
      // SystemID operator[](IndexType idx) { return systems_[idx]; }

      template <class SystemCollection>
      inline constexpr auto operator[](IndexType idx) -> SystemCollection& {
        return get_system_collection_container<SystemCollection>()[idx];
      }

      static constexpr SizeType get_number_of_system_collections() noexcept {
        return tmpl::size<AdmissibleSystemCollections>::value;
      }

      // TODO : Make private
      /*
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
      */
      /*
      template <class SystemBaseType, class System>
      inline void push_back_impl(System* system_ptr) {
        get_system_collection<SystemBaseType>().push_back(
            static_cast<SystemBaseType*>(system_ptr));
      }
       */
      template <class SystemCollection>
      inline auto push_back_impl(SystemCollection&& system)
          -> SystemCollection* {
        auto& system_collection_container =
            get_system_collection_container<SystemCollection>();
        system_collection_container.push_back(
            std::forward<SystemCollection>(system));
        // return non-owning handle to the System just pushed back
        return &system_collection_container.back();
      }

      // Vector like interface here, mixin?
      //      template <class System>
      //      inline void push_back(const SystemInterfaceType<System>& system) {

      // Only accepts forwarding references by design to own memory: don't push
      // back a const&
      template <class SystemCollection>
      inline auto push_back(SystemCollection&& system) {
        // TODO Push back dispatch to different vectors based on admissible
        // types (fixed, infinite: interact with Environment )
        //        using find_result =
        //            tmpl::find<AdmissibleSystems, std::is_base_of<tmpl::_1,
        //            System>>;
        //        using find_result =
        //            tmpl::find<AdmissibleSystemCollections,
        //                       std::is_same<SystemCollection, tmpl::_1>>;

        static_assert(
            tmpl::list_contains_v<AdmissibleSystemCollections,
                                  SystemCollection> != 0,
            "Cannot add the system to the simulator, as its type"
            "is not recognized while searching the list of valid systems. "
            "If this is not an error, consider extending the "
            "DefaultSimulatorConfiguration to add your "
            "system\n");

        //        using SystemBaseType = tmpl::front<find_result>;
        //        push_back_impl<SystemBaseType, System>(system.get());

        return push_back_impl(std::forward<SystemCollection>(system));

        //        static_assert(
        //            std::is_base_of<SystemBase, System>::value or
        //                          std::is_same<double, System>::value,
        //                      "Cannot add to simulator now boi");
        //        push_back_impl(system.get());
      }

      template <class... SystemCollectionsToBeAdded>
      inline decltype(auto) push_back(SystemCollectionsToBeAdded&&... system) {
        return std::make_tuple(
            push_back(std::forward<SystemCollectionsToBeAdded>(system))...);
      }

      template <class SystemCollection, class... Args>
      inline auto emplace_back_impl(Args&&... args) -> SystemCollection* {
        auto& system_collection_container =
            get_system_collection_container<SystemCollection>();
        system_collection_container.emplace_back(std::forward<Args>(args)...);
        // return non-owning handle to the System just pushed back
        return &system_collection_container.back();
      }

      // Preferred way of doing things
      template <class SystemCollection, class... Args>
      inline auto emplace_back(Args&&... args) {
        static_assert(
            tmpl::list_contains_v<AdmissibleSystemCollections,
                                  SystemCollection> != 0,
            "Cannot add the system to the simulator, as its type"
            "is not recognized while searching the list of valid systems. "
            "If this is not an error, consider extending the "
            "DefaultSimulatorConfiguration to add your "
            "system\n");

        return emplace_back_impl<SystemCollection>(std::forward<Args>(args)...);
      }

      // TODO : Emplace multiple
      //      template <class... SystemCollectionsToBeAdded>
      //      inline decltype(auto) emplace_back(SystemCollectionsToBeAdded&&...
      //      system) {
      //        return std::make_tuple(
      //            push_back(std::forward<SystemCollectionsToBeAdded>(system))...);
      //      }

      template <typename D = DomainType,
                Requires<std::is_same<D, PeriodicDomain>::value> = nullptr>
      void check_domain() {
        // https://stackoverflow.com/questions/12958925/expand-macros-inside-quoted-string
#define CHECK_DOMAIN_PERIODIC_ALONG(DIRECTION)                        \
  if (std::is_same<BoundaryTypeAlong<domain_.DIRECTION>,              \
                   PeriodicBoundary>::value and                       \
      (is_inf(domain_.template get_lower_bound<domain_.DIRECTION>) or \
       is_inf(domain_.template get_upper_bound<domain_.DIRECTION>)))  \
    throw std::runtime_error(                                         \
        "DomainType is set to be Periodic along " #DIRECTION          \
        " yet span of the domain along is unset (or) infinite "       \
        "Please call the simulators set_domain method with finite "   \
        "values in the periodic direction "                           \
        "before the call to integrate() or finalize()!");

        CHECK_DOMAIN_PERIODIC_ALONG(X);
        CHECK_DOMAIN_PERIODIC_ALONG(Y);
        CHECK_DOMAIN_PERIODIC_ALONG(Z);

#undef CHECK_DOMAIN_PERIODIC_ALONG
      }

      template <typename D = DomainType,
                Requires<std::is_same<D, UnboundedDomain>::value> = nullptr>
      void check_domain() {}

      void finalize(TimeType time_v = 0.0) {
        // Travel up the hierarchy and call finalize on all
        EXPAND_PACK_LEFT_TO_RIGHT(
            Bases<Simulator<SimTraits, Bases...>, SimTraits>::finalize(time_v));

        // If domain is not set when the DomainType is periodic, cry for
        // attention
        check_domain();

        // Set ready
        ready_for_simulation_ = true;
      }

      // Set domain
      template <typename... Args>
      void set_domain(Args&&... args) noexcept {
        domain_.set(std::forward<Args>(args)...);
      }

      auto ready() const noexcept -> bool { return ready_for_simulation_; }

      ~Simulator(){
          // LOG_INFO_SECTION(log){
          // log << "Successfully destroyed the simulation instance";
          //}

          // TODO
          // As we decide to own memory, we need to manually clear
          // the pointers and references
      };

     private:
      //      class Simulator : public DefaultInterface<SimTraits>,
      //                        private Singleton<Simulator<SimTraits,
      //                        Bases...>, logging::Logger>, public
      //                        Bases<Simulator<SimTraits, Bases...>,
      //                        SimTraits>... {

      // NOTE : Ensure singleton instance is constructed first to propagate
      // instance to other base classes
      Simulator()
          : Singleton<Simulator<SimTraits, Bases...>, logging::Logger>(),
            DefaultInterface<SimTraits>(),
            Bases<Simulator<SimTraits, Bases...>, SimTraits>()...,
            ready_for_simulation_(false),
            domain_() {
        //        LOG_INFO_SECTION(log) {
        //          log << "Successfully initialized the simulation instance";
        //        }
      }

      BEFRIEND_SINGLETON;

      template <template <typename /*Simulator*/, typename /*SimTraits*/>
                class... AddOns,
                typename SimulationOptions>
      friend auto make_simulator(SimulationOptions);
      // TODO Friend the get functions

      /*
      int get(std::size_t idx){
      return d[idx];
    }
      */

      SystemCollectionStorage system_collection_containers_;
      bool ready_for_simulation_;
      DomainType domain_;
      //  std::vector<SystemID> systems_;
      // TODO : Delete, purely for testing purposes
      //  std::vector<double*> double_systems_;
    };  // namespace interface

  }  // namespace interface

}  // namespace elastica

namespace std {

  template <typename System, typename SimTraits,
            template <typename /*Simulator*/, typename /*SimTraits*/>
            class... Bases>
  inline constexpr auto& get(
      ::elastica::interface::Simulator<SimTraits, Bases...>& simulator) {
    return simulator.template get_system_collection_container<System>();
  }

  template <std::size_t SystemCollectionIdx, typename SimTraits,
            template <typename /*Simulator*/, typename /*SimTraits*/>
            class... Bases>
  inline constexpr auto& get(
      ::elastica::interface::Simulator<SimTraits, Bases...>& simulator) {
    return simulator
        .template get_system_collection_container<SystemCollectionIdx>();
  }

}  // namespace std
