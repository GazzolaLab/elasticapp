//
// Created by Tejaswin Parthasarathy on 9/25/21.
//

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/CollisionSystem/Batches/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Configuration.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/RigidBodyPrimitiveTrait.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Coarse/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Detection/Fine/Types.hpp"
#include "Simulator/Environments/CollisionSystem/Response/Types.hpp"

namespace {

  using namespace elastica::collision::detection::coarse;

  using FirstTrait = ::elastica::collision::CollisionSystemConfiguration<
      ExhaustiveSearch, ::elastica::collision::detection::fine::MaxContacts,
      ::elastica::collision::batches::SingleBatch,
      ::elastica::collision::response::DEMSolver>;
  using SecondTrait = ::elastica::collision::CollisionSystemConfiguration<
      HashGrids, ::elastica::collision::detection::fine::MaxContacts,
      ::elastica::collision::batches::SingleBatch,
      ::elastica::collision::response::DEMSolver>;

  template <typename C>
  class Testing : public RigidBodyPrimitiveTrait<C> {
   public:
    using Parent = RigidBodyPrimitiveTrait<C>;
    template <typename... Args>
    explicit Testing(Args&&... args) : Parent(std::forward<Args>(args)...) {}
    using typename Parent::AABB;

    AABB& get_aabb() noexcept { return Parent::aabb_; }
  };

  // clang-format off
  ELASTICA_TEST_CASE(
  "Unit.Environments.CoarseDetection.RigidBodyPrimitiveTrait.ExhaustiveSearch",
      "[Unit][Environments]") {
    // clang-format on
    EMPTY_TEST();

    void* p = nullptr;
    Testing<FirstTrait> testing(static_cast<::elastica::collision::BodyID>(p));

    testing.get_aabb()[0] = 0.2;
    testing.get_aabb()[1] = 0.3;
    testing.get_aabb()[2] = 0.4;
    testing.get_aabb()[3] = 1.0;
    testing.get_aabb()[4] = 0.9;
    testing.get_aabb()[5] = 0.8;
  }

  ELASTICA_TEST_CASE(
      "Unit.Environments.CoarseDetection.RigidBodyPrimitiveTrait.HashGrids",
      "[Unit][Environments]") {
    void* p = nullptr;
    Testing<SecondTrait> testing(static_cast<::elastica::collision::BodyID>(p));

    testing.get_aabb()[0] = 0.2;
    testing.get_aabb()[1] = 0.3;
    testing.get_aabb()[2] = 0.4;
    testing.get_aabb()[3] = 1.0;
    testing.get_aabb()[4] = 0.9;
    testing.get_aabb()[5] = 0.8;

    CHECK(testing.get_AABB_size() == approx(0.8));
  }

}  // namespace
