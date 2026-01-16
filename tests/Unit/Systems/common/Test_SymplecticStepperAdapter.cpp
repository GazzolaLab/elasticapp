//
// Created by Tejaswin Parthasarathy on 10/5/21.
//

#include "Framework/TestingFramework.hpp"
#include "Systems/Block.hpp"
#include "Systems/common/SymplecticStepperAdapter.hpp"
#include "Systems/common/Tags.hpp"
#include "Time/Time.hpp"
#include "Utilities/CRTP.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"

namespace {
  using namespace elastica;

  struct DummyRank {
    using type = float;
    using slice_type = float&;
    using const_slice_type = float const&;
  };

  // copy pasted the plugin from Test_BlockFacade.cpp
  template <typename Tag>
  struct DummyVariable : public ::blocks::Variable<Tag, DummyRank> {};

  template <typename /*Traits*/, typename B>
  struct TestPlugin : public ::elastica::CRTPHelper<B, TestPlugin> {
    // Declare the variables
    // !! New !! Everything is a vector now!
    struct Position : DummyVariable<tags::Position> {};
    struct Velocity : DummyVariable<tags::Velocity> {};
    struct Acceleration : DummyVariable<tags::Acceleration> {};

    struct Director : DummyVariable<tags::Director> {};
    struct AngularVelocity : DummyVariable<tags::AngularVelocity> {};
    struct AngularAcceleration : DummyVariable<tags::AngularAcceleration> {};

    using ::elastica::CRTPHelper<B, TestPlugin>::self;

    // All variables
    using Variables = tmpl::list<Position, Velocity, Acceleration, Director,
                                 AngularVelocity, AngularAcceleration>;
    using InitializedVariables = tmpl::list<>;
    using ComputedVariables = tmpl::list<>;

    float& get_position() { return tuples::get<Position>(self().data()); }
    float& get_velocity() { return tuples::get<Velocity>(self().data()); }
    float& get_acceleration() {
      return tuples::get<Acceleration>(self().data());
    }
    float& get_director() { return tuples::get<Director>(self().data()); }
    float& get_angular_velocity() {
      return tuples::get<AngularVelocity>(self().data());
    }
    float& get_angular_acceleration() {
      return tuples::get<AngularAcceleration>(self().data());
    }

    float const& get_position() const {
      return tuples::get<Position>(self().data());
    }
    float const& get_velocity() const {
      return tuples::get<Velocity>(self().data());
    }
    float const& get_acceleration() const {
      return tuples::get<Acceleration>(self().data());
    }
    float const& get_director() const {
      return tuples::get<Director>(self().data());
    }
    float const& get_angular_velocity() const {
      return tuples::get<AngularVelocity>(self().data());
    }
    float const& get_angular_acceleration() const {
      return tuples::get<AngularAcceleration>(self().data());
    }
  };

  template <typename Traits, typename B>
  void update_acceleration(TestPlugin<Traits, B>&) noexcept {};

  template <typename T, template <typename> class B,
            template <typename, typename> class Plugin>
  struct PluginHelper
      : public ::elastica::SymplecticPolicy<T, B<PluginHelper<T, B, Plugin>>,
                                            Plugin> {};

  struct MockTraits {};

  template <template <typename> class B>
  using BoundTestPlugin = PluginHelper<MockTraits, B, TestPlugin>;

}  // namespace

namespace blocks {

  template <>
  class Block<BoundTestPlugin<Block>>
      : public BlockFacade<BoundTestPlugin<Block>> {
   public:
    //////////////////////////// start conformance
    ////////////////////////////////

    //    // In this case, we do not check the business logic in this class, but
    //    // only if the interfaces are OK.
    //    inline std::size_t check(std::size_t /* index */) const noexcept {
    //      return 0UL;
    //    }
    //    inline std::size_t check(elastica::from_end /* index */) const
    //    noexcept {
    //      return 0UL;
    //    };

    // customization point for generating a slice. Since all are vectors, a
    // simple indexing should do
    template <typename Tag>
    inline constexpr typename Tag::slice_type slice(std::size_t /* index */) & {
      return tuples::get<Tag>(this->data());
    }
    template <typename Tag>
    inline constexpr typename Tag::const_slice_type slice(
        std::size_t /* index */) const& {
      return tuples::get<Tag>(this->data());
    }
    ///////////////////////////// end conformance
    /////////////////////////////////

    Block() = default;
    ~Block() = default;
  };

}  // namespace blocks

namespace elastica {

  namespace states {

    //    // assign version doesnt care about vectorization
    //    auto SO3_primitive_assign(float& lhs, float const& rhs_matrix,
    //                              float const& rhs_vector) noexcept -> void {
    //      lhs += rhs_matrix + rhs_vector;
    //    };

    // add assign version doesnt care about vectorization
    template <typename LHSMatrix, typename RHSVector>
    auto SO3_primitive_add_assign(LHSMatrix& lhs,
                                  RHSVector const& rhs_vector) noexcept
        -> void {
      lhs += rhs_vector;
    }
    template <>
    inline auto size_backend<float>(float const&) noexcept -> std::size_t {
      return 1UL;
    }
    template <>
    inline auto resize_backend<float>(float&, std::size_t) -> void{};

  }  // namespace states

}  // namespace elastica

namespace {

  ELASTICA_TEST_CASE("Unit.Systems.SymplecticStepperAdapter",
                     "[Systems][Unit]") {
    using Plugin = BoundTestPlugin<blocks::Block>;
    using MyBlock = blocks::Block<Plugin>;
    MyBlock block;

    block.get_position() = 1.F;
    block.get_velocity() = 2.F;
    block.get_acceleration() = 5.F;

    block.get_director() = 3.F;
    block.get_angular_velocity() = 4.F;
    block.get_angular_acceleration() = 5.F;

    ::elastica::Time time_v{};

    // my god. what unreadable code.
    {
      constexpr std::size_t group = 0ul;  // checks the translation SE3 group
      CHECK(block.kinematic_states().get<group>().get() == 1.0F);
      CHECK(block.dynamic_states().get<group>().get() == 2.0F);
      CHECK(std::as_const(block).kinematic_rates(time_v).get<group>().get() ==
            2.0F);
      CHECK(/*std::as_const*/ (block)
                .dynamic_rates(time_v)
                .get<group>()
                .get() == 5.0F);
    }

    {
      constexpr std::size_t group = 1ul;  // checks the translation SE3 group
      CHECK(block.kinematic_states().get<group>().get() == 3.0F);
      CHECK(block.dynamic_states().get<group>().get() == 4.0F);
      CHECK(std::as_const(block).kinematic_rates(time_v).get<group>().get() ==
            4.0F);
      CHECK(/*std::as_const*/ (block)
                .dynamic_rates(time_v)
                .get<group>()
                .get() == 5.0F);
    }

    {
      // mimic a kinematic step
      ::elastica::Time time_v{10.0};
      ::elastica::TimeDelta time_delta{1.0};
      block.kinematic_states() += time_delta * block.kinematic_rates(time_v);

      CHECK(block.kinematic_states().get<0UL>().get() ==
            1.0 + 2.0 * double(time_delta));
      CHECK(block.kinematic_states().get<1UL>().get() ==
            3.0 + 4.0 * double(time_delta));
    }

    //    CHECK(block.kinematic_states().get() == 1);
    //    CHECK(block.dynamic_states().get() == 2);
    //    CHECK(std::as_const(block).kinematic_rates(time_v).get() == 2);
    //    CHECK(std::as_const(block).dynamic_rates(time_v).get() == 5);
  }

}  // namespace