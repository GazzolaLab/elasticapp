//
// Created by Tejaswin Parthasarathy on 4/24/22.
//
#include "Systems/RigidBody/Components.hpp"
#include "Systems/RigidBody/RigidBodyPlugin.hpp"
#include "Systems/RigidBody/RigidBodyTraits.hpp"
///
///
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  using namespace elastica;
  using namespace elastica::rigid_body;
}  // namespace

namespace {

  template <template <typename>
            class ComputationalBlock,  // Block with implementation
            template <typename /*CRT*/, typename /* ComputationalBlock */>
            class... Components>  // Skills
  class TestRigidBodyPlugin
      : public RigidBodyPlugin<DefaultRigidBodyTraits, ComputationalBlock,
                               Components...> {};

}  // namespace

namespace {

  template <typename D>
  struct DummyBlock : public D {};

  using PluginsToBeTested =
      tmpl::list<TestRigidBodyPlugin<DummyBlock, WithRigidBodyGeometry>,
                 TestRigidBodyPlugin<DummyBlock, WithRigidBodyGeometry,
                                     WithRigidBodyKinematics>,
                 TestRigidBodyPlugin<DummyBlock, WithSphereGeometry>,
                 TestRigidBodyPlugin<DummyBlock, WithSphereGeometry,
                                     WithRigidBodyKinematics>>;

  template <typename Plugin>
  struct ReportPluginProtocolConformation : std::true_type {
    static_assert(::tt::assert_conforms_to<Plugin, ::blocks::protocols::Plugin>,
                  "Plugin above does not conform to the protocols "
                  "expected by a blocks Plugin!");
  };

  using plugin_checks IGNORE_UNUSED =
      tmpl::transform<PluginsToBeTested,
                      ReportPluginProtocolConformation<tmpl::_1>>;

}  // namespace
