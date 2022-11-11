//
// Created by Tejaswin Parthasarathy on 4/27/22.
//
#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/Primitives.hpp"
#include "Simulator/Interface/Environments/Emplacer.hpp"
//
#include "Simulator/EnableReset.hpp"
#include "Simulator/Interface/Environments.hpp"
#include "Simulator/Simulator.hpp"
//
#include "Simulator/Configuration.hpp"

// use cosserat rod and rigid body here
#include "Systems/Systems.hpp"
//
#include "Utilities/CRTP.hpp"
#include "Utilities/CountingIterator.hpp"
#include "Utilities/NoSuchType.hpp"
#include "Utilities/PrettyType.hpp"
#include "Utilities/Size.hpp"
#include "Utilities/TMPL.hpp"

/*
 * This is an integrated test masquerading as a unit test
 */

//.......................................... copied from module test
namespace {

  //////// The following code block is copy pasted from another source
  template <typename Traits, typename Block>
  struct AlwaysNeededComponent
      : public ::elastica::CRTPHelper<Block, AlwaysNeededComponent> {
    using CRTP = ::elastica::CRTPHelper<Block, AlwaysNeededComponent>;
    using CRTP::self;

    struct Position : public Traits::template CosseratRodVariable<
                          ::elastica::tags::Position,         //
                          typename Traits::DataType::Vector,  //
                          typename Traits::Place::OnNode> {};
    struct Director : public Traits::template CosseratRodInitializedVariable<
                          ::elastica::tags::Director,  //
                          typename Traits::DataType::Matrix,
                          typename Traits::Place::OnElement> {};
    struct Velocity : public Traits::template CosseratRodInitializedVariable<
                          ::elastica::tags::Velocity,         //
                          typename Traits::DataType::Vector,  //
                          typename Traits::Place::OnNode> {};
    struct AngularVelocity
        : public Traits::template CosseratRodInitializedVariable<
              ::elastica::tags::AngularVelocity,  //
              typename Traits::DataType::Vector,
              typename Traits::Place::OnElement> {};
    struct Acceleration
        : public Traits::template CosseratRodInitializedVariable<
              ::elastica::tags::Acceleration,     //
              typename Traits::DataType::Vector,  //
              typename Traits::Place::OnNode> {};
    struct AngularAcceleration
        : public Traits::template CosseratRodInitializedVariable<
              ::elastica::tags::AngularAcceleration,  //
              typename Traits::DataType::Vector,
              typename Traits::Place::OnElement> {};
    struct Material
        : public Traits::template CosseratRodInitializedVariable<
              ::elastica::tags::Material,        //
              typename Traits::DataType::Index,  // actually should be a
              // different type, but whatever
              typename Traits::Place::OnElement> {};

    inline constexpr decltype(auto) get_position() & noexcept {
      return blocks::get<::elastica::tags::Position>(self());
    }
    inline constexpr decltype(auto) get_position() const& noexcept {
      return blocks::get<::elastica::tags::Position>(self());
    }

    inline constexpr decltype(auto) get_director() & noexcept {
      return blocks::get<::elastica::tags::Director>(self());
    }
    inline constexpr decltype(auto) get_director() const& noexcept {
      return blocks::get<::elastica::tags::Director>(self());
    }

    inline constexpr decltype(auto) get_velocity() & noexcept {
      return blocks::get<::elastica::tags::Velocity>(self());
    }
    inline constexpr decltype(auto) get_velocity() const& noexcept {
      return blocks::get<::elastica::tags::Velocity>(self());
    }

    inline constexpr decltype(auto) get_angular_velocity() & noexcept {
      return blocks::get<::elastica::tags::AngularVelocity>(self());
    }
    inline constexpr decltype(auto) get_angular_velocity() const& noexcept {
      return blocks::get<::elastica::tags::AngularVelocity>(self());
    }
    inline constexpr decltype(auto) get_acceleration() & noexcept {
      return blocks::get<::elastica::tags::Acceleration>(self());
    }
    inline constexpr decltype(auto) get_acceleration() const& noexcept {
      return blocks::get<::elastica::tags::Acceleration>(self());
    }
    inline constexpr decltype(auto) get_angular_acceleration() & noexcept {
      return blocks::get<::elastica::tags::AngularAcceleration>(self());
    }
    inline constexpr decltype(auto) get_angular_acceleration() const& noexcept {
      return blocks::get<::elastica::tags::AngularAcceleration>(self());
    }

    inline constexpr decltype(auto) get_material() & noexcept {
      return blocks::get<::elastica::tags::Material>(self());
    }
    inline constexpr decltype(auto) get_material() const& noexcept {
      return blocks::get<::elastica::tags::Material>(self());
    }

    inline constexpr decltype(auto) get_material(std::size_t idx) & noexcept {
      return Material::slice(get_material(), idx);
    }
    inline constexpr decltype(auto) get_material(
        std::size_t idx) const& noexcept {
      return Material::slice(get_material(), idx);
    }

    using ComputedVariables =
        tmpl::list<Position, Director, Velocity, AngularVelocity, Acceleration,
                   AngularAcceleration, Material>;
  };
  ////////

  template <typename T, typename B>
  struct EmptyComponent
      : public AlwaysNeededComponent<T, B>,
        public elastica::GeometryComponent<EmptyComponent<T, B>> {
    using P = AlwaysNeededComponent<T, B>;
    using InitializedVariables = tmpl::list<>;
    using typename P::ComputedVariables;
    using Variables = ComputedVariables;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock& db, CosseratInitializer&&) {
      db.get_material() = ::elastica::iron;
    }
  };

  template <typename T, typename B>
  struct EmptyComponentButWithADifferentName
      : public AlwaysNeededComponent<T, B>,
        public elastica::GeometryComponent<
            EmptyComponentButWithADifferentName<T, B>> {
    using P = AlwaysNeededComponent<T, B>;
    using InitializedVariables = tmpl::list<>;
    using typename P::ComputedVariables;
    using Variables = ComputedVariables;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock& db, CosseratInitializer&&) {
      db.get_material() = ::elastica::copper;
    }
  };

  // to test for_each() over a subset of systems
  template <typename T, typename B>
  struct RigidBodyEmptyComponent
      : public AlwaysNeededComponent<T, B>,
        public elastica::GeometryComponent<RigidBodyEmptyComponent<T, B>> {
    using P = AlwaysNeededComponent<T, B>;
    using InitializedVariables = tmpl::list<>;
    using typename P::ComputedVariables;
    using Variables = ComputedVariables;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock& db, CosseratInitializer&&) {
      db.get_material() = ::elastica::fir;
    }
  };

  template <typename T, typename B>
  struct NoCollisionComponent
      : public AlwaysNeededComponent<T, B>,
        public elastica::GeometryComponent<NoCollisionComponent<T, B>> {
    using P = AlwaysNeededComponent<T, B>;
    using InitializedVariables = tmpl::list<>;
    using typename P::ComputedVariables;
    using Variables = ComputedVariables;

    template <class DownstreamBlock, class CosseratInitializer>
    static void initialize(DownstreamBlock& db, CosseratInitializer&&) {
      db.get_material() = ::elastica::iron;
    }
  };

}  // namespace
//.......................................... copied from module test

namespace {

  template <template <typename, typename> class... T>
  using GenerateCRPlugin = ::elastica::cosserat_rod::CosseratRodPlugin<
      ::elastica::cosserat_rod::DefaultCosseratRodTraits, ::blocks::Block,
      T...>;
  template <template <typename, typename> class... T>
  using GenerateRBPlugin = ::elastica::rigid_body::RigidBodyPlugin<
      ::elastica::rigid_body::DefaultRigidBodyTraits, ::blocks::Block, T...>;

  template <typename Plugin>
  using GenerateBlock = blocks::Block<Plugin>;

  template <typename Block>
  using GetPluginFromBlock = typename Block::PluginType;

  using ECPlugin = GenerateCRPlugin<EmptyComponent>;
  using ECNamedPlugin = GenerateCRPlugin<EmptyComponentButWithADifferentName>;
  using RBPlugin = GenerateCRPlugin<RigidBodyEmptyComponent>;
  using NCPlugin = GenerateCRPlugin<NoCollisionComponent>;

  using ECBlock = GenerateBlock<ECPlugin>;
  using ECNamedBlock = GenerateBlock<ECNamedPlugin>;
  using RBBlock = GenerateBlock<RBPlugin>;
  using NCBlock = GenerateBlock<NCPlugin>;

}  // namespace

namespace elastica {

  namespace customization {

    template <typename T, typename B>
    struct PrimitiveTrait<EmptyComponent<T, B>> {
      using type = ::elastica::collision::CapsulePrimitive;
    };
    template <typename T, typename B>
    struct PrimitiveTrait<EmptyComponentButWithADifferentName<T, B>> {
      using type = ::elastica::collision::CapsuleFromRodPrimitive;
    };
    template <typename T, typename B>
    struct PrimitiveTrait<RigidBodyEmptyComponent<T, B>> {
      using type = ::elastica::collision::SpherePrimitive;
    };

    template <typename T, typename B>
    struct PrimitiveTrait<NoCollisionComponent<T, B>> {
      using type = NoSuchType;
    };

  }  // namespace customization

}  // namespace elastica

namespace {

  // NCPLugin has no such type : improper use in emplacer would trigger
  // compilation error.
  template <typename ListOfPlugins =
                tmpl::list<ECPlugin, ECNamedPlugin, RBPlugin, NCPlugin>,
            typename T, typename U>
  inline auto make_configuration(T across, U over) {
    using namespace ::elastica::configuration;
    using ListOfImposedPrimitives = tmpl::list<>;

    auto env_config =
        make_environments_configuration<ListOfImposedPrimitives>(seq);

    auto sys_config = make_systems_configuration<ListOfPlugins>(
        // this does not matter
        make_blocking_policy(RestrictSizeAcrossBlockTypes{20UL}),
        make_iteration_policy(iterate_across_system_types_in(std::move(across)),
                              // for testing iteration policy
                              iterate_over_systems_in(std::move(over))));
    return make_simulator_configuration(
        std::move(sys_config), DefaultDomainConfiguration{},
        make_parallel_configuration(4UL), std::move(env_config));
  }

  template <typename... Args>
  inline decltype(auto) make_unit_simulator(Args... args) {
    //
    return elastica::make_simulator<::elastica::modules::Environments,
                                    ::elastica::modules::EnableReset>(
        make_configuration<>(std::move(args)...));
  }

  template <typename... Args>
  inline decltype(auto) setup_sim_with_state(const std::size_t n_units,
                                             Args... args) {
    const std::size_t n_elements = 16UL;

    auto sim = make_unit_simulator(std::move(args)...);
    sim->reset();  // for some reason this is needed, else it complains about
                   // reset

    using ListOfSystems =
        typename decltype(sim)::element_type::ListOfAdmissibleSystemCollections;
    // gives n_units / 2 blocks of ECBlock and n_units ECNamedBlock
    tmpl::for_each<ListOfSystems>([&](auto p) {
      using Block = tmpl::type_from<decltype(p)>;
      using Plugin = GetPluginFromBlock<Block>;
      for (auto i = 0UL; i < n_units; ++i)
        sim->template emplace_back<Plugin>(
            ::elastica::cosserat_rod::initialize_cosserat_rod<Plugin>(
                blocks::initialize<::elastica::tags::NElement>(
                    [=](...) { return n_elements; })));
    });
    return sim;
  }

  template <typename... Args>
  void test_emplacer(Args... args) {
    const std::size_t n_units = 4UL;
    auto sim = setup_sim_with_state(n_units, std::move(args)...);

    // now check for emplaced values
    using ::elastica::collision::get_collection;
    using CP = ::elastica::collision::CapsulePrimitive;
    using CRP = ::elastica::collision::CapsuleFromRodPrimitive;
    using SP = ::elastica::collision::SpherePrimitive;

    auto& primitives = sim->primitives();

    auto& cp = get_collection<CP>(primitives);
    CHECK(::elastica::size(cp) == 0UL);

    auto& crp = get_collection<CRP>(primitives);
    CHECK(::elastica::size(crp) == 0UL);

    auto& sp = get_collection<SP>(primitives);
    CHECK(::elastica::size(sp) == 0UL);

    sim->finalize();
    sim->populate_primitives();

    {  // size check
      CHECK(::elastica::size(cp) == 64UL);
      CHECK(::elastica::size(crp) == 64UL);
      CHECK(::elastica::size(sp) == 64UL);
    }

    {
      // info check
      using std::cbegin;
      using std::cend;
      auto check_for = [](::elastica::MaterialID mid) {
        return [=](auto const& p) { CHECK(p.get_material() == mid); };
      };
      std::for_each(cbegin(cp), cend(cp), check_for(::elastica::iron));
      std::for_each(cbegin(crp), cend(crp), check_for(::elastica::copper));
      std::for_each(cbegin(sp), cend(sp), check_for(::elastica::fir));
    }

    {
      // offsets map check
      // There are 4 blocks with 16 elements each, so offsets should be
      // 0, 16, 32, 48
      auto& offsets = sim->offsets();
      tmpl::for_each<tmpl::list<ECBlock, ECNamedBlock, RBBlock>>([&](auto b) {
        using ::elastica::get_offset;
        using std::cbegin;
        using std::begin;
        using std::cend;

        using B = tmpl::type_from<decltype(b)>;
        auto& offset = get_offset<B>(offsets);
        std::set<std::size_t> indices;

        std::transform(cbegin(offset), cend(offset),
                       std::inserter(indices, begin(indices)),
                       [](const auto& p) { return p.second; });

        std::for_each(cbegin(indices), cend(indices),
                      [index = 0UL](auto const p) mutable {
                        CHECK(p == (index * 16UL));
                        ++index;
                      });
      });
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.Simulator.Interface.Environments.Emplacer",
                     "[Unit][Simulator][Interface][Environments]") {
    // clang-format on
    test_emplacer(::elastica::configuration::seq,
                  ::elastica::configuration::seq);

    using Configurations =
        tmpl::list<tmpl::pair<::elastica::configuration::parallel_policy,
                              ::elastica::configuration::sequential_policy>,
                   tmpl::pair<::elastica::configuration::sequential_policy,
                              ::elastica::configuration::parallel_policy>,
                   tmpl::pair<::elastica::configuration::parallel_policy,
                              ::elastica::configuration::parallel_policy>>;
    tmpl::for_each<Configurations>([](auto conf) {
      using C = tmpl::type_from<decltype(conf)>;
      test_emplacer(typename C::first_type{}, typename C::second_type{});
    });
  }

}  // namespace

// Type customization to test transfer mechanism does no-op for NoSuchType.
namespace elastica {

  namespace cosserat_rod {

    namespace detail {

      struct NoCollisionEmplacerTag {};
      using NoCollisionEmplacerRod =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::Block, NoCollisionEmplacerTag,
              CosseratRodWithoutDampingComponents>;
      using NoCollisionEmplacerRodSlice =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::BlockSlice, NoCollisionEmplacerTag,
              CosseratRodWithoutDampingComponents>;
      using NoCollisionEmplacerRodConstSlice =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::ConstBlockSlice, NoCollisionEmplacerTag,
              CosseratRodWithoutDampingComponents>;
      using NoCollisionEmplacerRodView =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::BlockView, NoCollisionEmplacerTag,
              CosseratRodWithoutDampingComponents>;
      using NoCollisionEmplacerRodConstView =
          ::elastica::cosserat_rod::TaggedCosseratRodPlugin<
              ::elastica::cosserat_rod::DefaultCosseratRodTraits,
              ::blocks::ConstBlockView, NoCollisionEmplacerTag,
              CosseratRodWithoutDampingComponents>;

      using NoCollisionEmplacerRodBlock =
          ::blocks::Block<NoCollisionEmplacerRod>;
      using NoCollisionEmplacerRodBlockSlice =
          ::blocks::BlockSlice<NoCollisionEmplacerRodSlice>;
      using NoCollisionEmplacerRodBlockConstSlice =
          ::blocks::ConstBlockSlice<NoCollisionEmplacerRodConstSlice>;
      using NoCollisionEmplacerRodBlockView =
          ::blocks::BlockView<NoCollisionEmplacerRodView>;
      using NoCollisionEmplacerRodBlockConstView =
          ::blocks::ConstBlockView<NoCollisionEmplacerRodConstView>;

    }  // namespace detail

  }  // namespace cosserat_rod

}  // namespace elastica

namespace elastica {

  namespace customization {

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct PrimitiveTrait<
        ::elastica::cosserat_rod::detail::NoCollisionEmplacerRod> {
      using type = NoSuchType;
    };
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct PrimitiveTrait<
        ::elastica::cosserat_rod::detail::NoCollisionEmplacerRodBlock>
        : public PrimitiveTrait<
              ::elastica::cosserat_rod::detail::NoCollisionEmplacerRod> {};
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct PrimitiveTrait<
        ::elastica::cosserat_rod::detail::NoCollisionEmplacerRodBlockSlice>
        : public PrimitiveTrait<
              ::elastica::cosserat_rod::detail::NoCollisionEmplacerRod> {};
    /*! \endcond */
    //**************************************************************************

    //**************************************************************************
    /*! \cond ELASTICA_INTERNAL */
    template <>
    struct PrimitiveTrait<
        ::elastica::cosserat_rod::detail::NoCollisionEmplacerRodBlockConstSlice>
        : public PrimitiveTrait<
              ::elastica::cosserat_rod::detail::NoCollisionEmplacerRod> {};
    /*! \endcond */
    //**************************************************************************

  }  // namespace customization

}  // namespace elastica

namespace {

  template <typename... Args>
  inline decltype(auto) make_integrated_simulator(Args... args) {
    //
    using Systems =
        tmpl::list<::elastica::cosserat_rod::CosseratRod,
                   ::elastica::cosserat_rod::CosseratRodWithoutDamping,
                   ::elastica::cosserat_rod::detail::NoCollisionEmplacerRod,
                   ::elastica::rigid_body::Sphere>;
    return elastica::make_simulator<::elastica::modules::Environments,
                                    ::elastica::modules::EnableReset>(
        make_configuration<Systems>(std::move(args)...));
  }

  template <typename... Args>
  inline decltype(auto) setup_integrated_sim_with_state(
      const std::size_t n_units, Args... args) {
    auto sim = make_integrated_simulator(std::move(args)...);
    sim->reset();
    {
      using Plugin = ::elastica::cosserat_rod::CosseratRod;

      using Init = ::elastica::cosserat_rod::StraightCosseratRodInitializer;

      const std::size_t n_elements_per_slice = 16UL;

      for (auto i = 0UL; i < n_units; ++i) {
        Init initializer{Init::NElement{n_elements_per_slice},
                         Init::Material{::elastica::copper},
                         Init::Radius{0.2},
                         Init::Length{1.0},
                         Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
                         Init::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
                         Init::Normal{::elastica::Vec3{1.0, 0.0, 0.0}},
                         Init::ForceDampingRate{0.2},
                         Init::TorqueDampingRate{0.2}};

        sim->template emplace_back<Plugin>(initializer.initialize<Plugin>());
      }
    }

    {
      using Plugin = ::elastica::cosserat_rod::CosseratRodWithoutDamping;

      using Init = ::elastica::cosserat_rod::
          StraightCosseratRodWithoutDampingInitializer;

      const std::size_t n_elements_per_slice = 16UL;

      for (auto i = 0UL; i < n_units; ++i) {
        Init initializer{Init::NElement{n_elements_per_slice},
                         Init::Material{::elastica::iron},
                         Init::Radius{0.2},
                         Init::Length{1.0},
                         Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
                         Init::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
                         Init::Normal{::elastica::Vec3{1.0, 0.0, 0.0}}};

        sim->template emplace_back<Plugin>(initializer.initialize<Plugin>());
      }
    }

    {
      using Plugin = ::elastica::cosserat_rod::detail::NoCollisionEmplacerRod;

      using Init = ::elastica::cosserat_rod::
          StraightCosseratRodWithoutDampingInitializer;
      const std::size_t n_elements_per_slice = 8UL;

      for (auto i = 0UL; i < n_units; ++i) {
        Init initializer{Init::NElement{n_elements_per_slice},
                         Init::Material{::elastica::iron},
                         Init::Radius{0.2},
                         Init::Length{1.0},
                         Init::Origin{::elastica::Vec3{0.0, 0.0, 0.0}},
                         Init::Direction{::elastica::Vec3{0.0, 0.0, 1.0}},
                         Init::Normal{::elastica::Vec3{1.0, 0.0, 0.0}}};

        sim->template emplace_back<Plugin>(initializer.initialize<Plugin>());
      }
    }

    {
      using Plugin = ::elastica::rigid_body::Sphere;
      // this will result in 2 blocks with the Restrictionpolcy we have in place
      const std::size_t n_elements = 24UL;

      std::for_each(
          elastica::make_counting_iterator(0UL),
          elastica::make_counting_iterator(n_elements), [&](std::size_t) {
            auto initializer = ::blocks::initialize_block<Plugin>(
                ::blocks::initialize<::elastica::tags::Position>([]() {
                  return ::elastica::Vec3{1., 0., 0.};
                }),
                ::blocks::initialize<::elastica::tags::Director>(
                    []() -> ::elastica::Rot3 {
                      return 2.0 * ::elastica::identity3();
                    }),
                ::blocks::initialize<::elastica::tags::Radius>(
                    []() { return 0.3; }),
                ::blocks::initialize<::elastica::tags::Velocity>([]() {
                  return ::elastica::Vec3{1., 0., 0.};
                }),
                ::blocks::initialize<::elastica::tags::AngularVelocity>([]() {
                  return ::elastica::Vec3{1., 0., 0.};
                }),
                ::blocks::initialize<::elastica::tags::Acceleration>([]() {
                  return ::elastica::Vec3{1., 0., 0.};
                }),
                ::blocks::initialize<::elastica::tags::AngularAcceleration>(
                    []() {
                      return ::elastica::Vec3{1., 0., 0.};
                    }),
                ::blocks::initialize<::elastica::tags::Material>(
                    []() { return ::elastica::fir; }));

            sim->template emplace_back<Plugin>(std::move(initializer));
          });
    }

    return sim;
  }

  template <typename... Args>
  void test_integrated_emplacer(Args... args) {
    const std::size_t n_units = 4UL;
    auto sim = setup_integrated_sim_with_state(n_units, std::move(args)...);

    // now check for emplaced values
    using ::elastica::collision::get_collection;
    using CRP = ::elastica::collision::CapsuleFromRodPrimitive;
    using SP = ::elastica::collision::SpherePrimitive;

    auto& primitives = sim->primitives();

    auto& crp = get_collection<CRP>(primitives);
    CHECK(::elastica::size(crp) == 0UL);

    auto& sp = get_collection<SP>(primitives);
    CHECK(::elastica::size(sp) == 0UL);

    sim->finalize();
    if (sim->primitives_are_required()) {
      // it takes care of populating it
    } else {
      sim->populate_primitives();
    }

    {  // size check
      // 64 + 64 for both types of rod
      CHECK(::elastica::size(crp) == 64UL + 64UL);
      CHECK(::elastica::size(sp) == 24UL);
    }

    {
      // info check
      using std::cbegin;
      using std::cend;
      auto check_for = [](::elastica::MaterialID mid) {
        return [=](auto const& p) { CHECK(p.get_material() == mid); };
      };
      // auto myit = cbegin(crp) + static_cast<long>(64UL);
      // This order is dependent on execution, so we can only check for the
      // total number of coppers and irons
      auto counts = std::accumulate(
          cbegin(crp), cend(crp), std::make_pair(0UL, 0UL),
          [](auto accum, auto const& p) {
            return p.get_material() == ::elastica::copper
                       ? std::make_pair(std::get<0UL>(accum) + 1UL,
                                        std::get<1UL>(accum))
                       : std::make_pair(std::get<0UL>(accum),
                                        std::get<1UL>(accum) + 1UL);
          });
      CHECK(std::get<0UL>(counts) == 64UL);
      CHECK(std::get<1UL>(counts) == 64UL);

      // std::for_each(myit, , check_for(::elastica::iron));
      std::for_each(cbegin(sp), cend(sp), check_for(::elastica::fir));
    }

    {
      // offsets map check
      // There are 4 blocks with 16 elements each, so offsets should be
      // 0, 16, 32, 48
      auto& offsets = sim->offsets();
      std::set<std::size_t> capsule_indices;

      tmpl::for_each<
          tmpl::list<::blocks::Block<::elastica::cosserat_rod::CosseratRod>>>(
          [&](auto b) {
            using ::elastica::get_offset;
            using std::cbegin;
            using std::begin;
            using std::cend;

            using B = tmpl::type_from<decltype(b)>;
            auto& offset = get_offset<B>(offsets);

            std::transform(
                cbegin(offset), cend(offset),
                std::inserter(capsule_indices, begin(capsule_indices)),
                [](const auto& p) { return p.second; });
          });
      tmpl::for_each<tmpl::list<::blocks::Block<
          ::elastica::cosserat_rod::CosseratRodWithoutDamping>>>([&](auto b) {
        using ::elastica::get_offset;
        using std::cbegin;
        using std::begin;
        using std::cend;

        using B = tmpl::type_from<decltype(b)>;
        auto& offset = get_offset<B>(offsets);

        std::transform(cbegin(offset), cend(offset),
                       std::inserter(capsule_indices, begin(capsule_indices)),
                       [](const auto& p) { return p.second; });
      });

      // I don't know which takes precedence, so I compare at the end
      using std::cbegin;
      using std::cend;
      std::for_each(cbegin(capsule_indices), cend(capsule_indices),
                    [index = 0UL](auto const p) mutable {
                      CHECK(p == (index * 16UL));
                      ++index;
                    });

      tmpl::for_each<
          tmpl::list<::blocks::Block<::elastica::rigid_body::Sphere>>>(
          [&](auto b) {
            using ::elastica::get_offset;
            using std::cbegin;
            using std::begin;
            using std::cend;

            using B = tmpl::type_from<decltype(b)>;
            auto& offset = get_offset<B>(offsets);
            std::set<std::size_t> indices;

            std::transform(cbegin(offset), cend(offset),
                           std::inserter(indices, begin(indices)),
                           [](const auto& p) { return p.second; });

            std::for_each(cbegin(indices), cend(indices),
                          [index = 0UL](auto const p) mutable {
                            CHECK(p == (index * 20UL));
                            ++index;
                          });
          });
    }
  }

  // clang-format off
  ELASTICA_TEST_CASE("Unit.z.Integrated.Simulator.Interface.Environments.Emplacer",
                     "[Integrated][Simulator][Interface][Environments]") {
    // clang-format on
    using Configurations =
        tmpl::list<tmpl::pair<::elastica::configuration::sequential_policy,
                              ::elastica::configuration::sequential_policy>,
                   tmpl::pair<::elastica::configuration::parallel_policy,
                              ::elastica::configuration::sequential_policy>,
                   tmpl::pair<::elastica::configuration::sequential_policy,
                              ::elastica::configuration::parallel_policy>,
                   tmpl::pair<::elastica::configuration::parallel_policy,
                              ::elastica::configuration::parallel_policy>>;
    tmpl::for_each<Configurations>([](auto conf) {
      using C = tmpl::type_from<decltype(conf)>;
      SECTION(::pretty_type::get_name<C>()) {
        test_integrated_emplacer(typename C::first_type{},
                                 typename C::second_type{});
      }
    });
  }

}  // namespace
