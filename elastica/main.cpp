#include <iostream>
#include <new>
#include <string>
//#include <vector>

//#include "tbb/parallel_for.h"

//#include "blaze/Math.h"

//#include "DataStructures/Block.hpp"
//#include "DataStructures/StorageTraits.hpp"
//#include "DataStructures/TaggedTuple.hpp"
//#include "Time/History.hpp"
#include "Utilities/DefineTypes.h"
//#include "Utilities/Logging.hpp"
//#include "Utilities/Singleton.hpp"
//#include "Utilities/StdHelpers.hpp"
#include "Simulator/Environments/Primitives/PlanePrimitive.hpp"
#include "Simulator/Materials.hpp"

/*
int main(int argc, char* argv[]) {
  constexpr size_t Dimension = 3;
  constexpr size_t BlockSize = 10;
  using BlockMetaData =
      Blocks::BlockData<Dimension, BlockSize,
                        storage_detail::StorageOption::Static>;
  using toy_block = Blocks::Block<Groups::GenericBodyGroup, BlockMetaData>;
  toy_block sb;
  std::cout << std::boolalpha << Blocks::is_valid_block_v<toy_block> << '\n';
  TimeSteppers::History<toy_block,
                        tuples::item_data_type<Tags::Position, BlockMetaData>,
                        tuples::item_data_type<Tags::Velocity, BlockMetaData>>
      history;
  std::cout << history.capacity() + 1;
  //
  //  std::get<Tags::Position>(sb),
  //                        std::get<Tags::Velocity>(sb)>
  //      h;
  //  TaggedTuple<Tags::SimplePODTag> tt;
  //  using myt = decltype(tuples::get<Tags::Position, Tags::Velocity>(sb));
  //  using yot = decltype(std::get<Tags::Position>(sb));

  return 0;
}
*/

/*
class TestDep : public Singleton<TestDep> {
 private:
  explicit TestDep(){};

 public:
  ~TestDep(){};

 private:
  BEFRIEND_SINGLETON;
};

class TestSingle : private Singleton<TestSingle, TestDep> {
 private:
  explicit TestSingle() { a = 2; };

 public:
  ~TestSingle(){};
  int a;
  friend inline std::shared_ptr<TestSingle> TheTestSingle();

 private:
  BEFRIEND_SINGLETON;
};

inline std::shared_ptr<TestSingle> TheTestSingle() {
  return TestSingle::instance();
}

int main(int argc, char* argv[]) {
  auto ts = TheTestSingle();
  LOG_INFO_SECTION(log) { log << "A IS " << ts->a << "BOIII"; }
  LOG_ERROR_SECTION(log) { log << "Thick BOIII"; }
}
*/
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/DynamicVector.h>
#include <iomanip>

#include "LocalSystems.hpp"
#include "elastica/Simulator/Environments.hpp"
#include "elastica/Simulator/IO/csv.hpp"
#include "elastica/Simulator/IO/vtk.hpp"
#include "elastica/Simulator/Simulator.hpp"
#include "elastica/Simulator/Trigger.hpp"
#include "elastica/Simulator/Visualization.hpp"
#include "elastica/Systems/RigidBodyBase.hpp"
#include "elastica/Time/integrate.hpp"
#include "elastica/Time/symplectic.hpp"
#include "elastica/Utilities/DefineTypes.h"
#include "elastica/Utilities/TMPL.hpp"
#include "elastica/Utilities/Timing/WcTimer.h"

using namespace elastica;
template <class TimeStepper, class SystemCollection, typename TimeType>
void integrate(TimeStepper ts, SystemCollection& sys, TimeType final_time,
               std::size_t n_steps = 1000UL) {
  TimeType time_v(0.0);
  TimeType dt(final_time / static_cast<TimeType>(n_steps));
  std::vector<timing::WcTimer*> timers_;
  timing::WcTimer timeDoStep_, timeDtIncrement_;
  timers_.push_back(&timeDoStep_);
  timers_.push_back(&timeDtIncrement_);
  if (!sys.ready())
    sys.finalize();
  for (std::size_t i = 0UL; i < n_steps; ++i) {
    timeDoStep_.start();
    ts.do_step(sys, time_v, dt);
    timeDoStep_.stop();
    timeDtIncrement_.start();
    time_v += dt;
    timeDtIncrement_.stop();
    // Triggering all registered trigger objects
    Trigger::trigger_all(time_v, i);
    std::cout << "Macha, one pass done dude at time " << time_v << std::endl;
  }
  // Profiling section
  std::vector<double> minValues(timers_.size()), maxValues(timers_.size()),
      totalValues(timers_.size());
  std::vector<size_t> numValues(timers_.size());
  for (std::size_t i = 0; i < minValues.size(); ++i) {
    minValues[i] = timers_[i]->min();
    maxValues[i] = timers_[i]->max();
    totalValues[i] = timers_[i]->total();
    numValues[i] = timers_[i]->getCounter();
  }
  LOG_INFO_SECTION(log) {
    // clang-format off
  log << "Timing results reduced over all time steps on current process:\n" << std::fixed << std::setprecision(4)
	  << "code part              min time     max time     avg time     total time   executions\n"
	  << "--------------------   ----------   ----------   ----------   ----------   ----------\n"
	  << "simulation step        "   << std::setw(10) << minValues[0] << "   " << std::setw(10) << maxValues[0] << "   " << std::setw(10) << totalValues[0] / numValues[0] << " = " << std::setw(10) << totalValues[0] << " / " << std::setw(10) << numValues[0] << "\n"
	  << "   dt. increment       "   << std::setw(10) << minValues[ 1] << "   " << std::setw(10) << maxValues[ 1] << "   " << std::setw(10) << totalValues[ 1] / numValues[ 1] << " = " << std::setw(10) << totalValues[ 1] << " / " << std::setw(10) << numValues[ 1] << "\n";
    // clang-format on
  }
}

// DONE : Traits class for simulator and all its Interface classes
int main(int argc, char* argv[]) {
  //  using namespace elastica::systems;
  //  using namespace elastica::rigidbody;
  //  auto sim(make_simulator<Constraints, Connections>());
  auto sim(make_simulator<Environments>());

  //  auto sho_one = std::make_shared<SHO<double>>(3.0);
  //  sim->push_back(sho_one);
  //  auto sho_two = std::make_shared<SHO<double>>(6.0);
  //  sim->push_back(sho_two);
  //  auto ark_one = std::make_shared<ARKAnalytic<double>>(-10.0);
  //  sim->push_back(ark_one);

  constexpr int case_number = 0;

  using MT = blaze::DynamicMatrix<real_t>;
  using VT = blaze::DynamicVector<real_t, blaze::rowVector>;

  if (case_number == 0) {
    std::size_t n_x(5UL);
    std::size_t n_y(5UL);

    std::size_t n_spheres(n_x * n_y);
    MT positions(3UL, n_spheres, 0.0);

    // Bug in blaze, for linspacece(2UL, -25.0, 25.0) yields (25.0, 25.0)
    VT x_grid = blaze::linspace<blaze::rowVector>(n_x, -25.0, 25.0);
    VT y_grid = blaze::linspace<blaze::rowVector>(n_y, -25.0, 25.0);

    //  VT x_grid{-25.0, 25.0};
    //  VT y_grid{-25.0, 25.0};
    std::cout << x_grid << std::endl;
    std::cout << y_grid << std::endl;
    for (std::size_t i = 0; i < n_spheres; ++i) {
      positions(0UL, i) = x_grid[i % n_x];
      positions(1UL, i) = y_grid[i / n_y];
    }

    blaze::row(positions, 2UL) = 1.0;
    MT velocities(3UL, n_spheres, 0.0);

    // TO test out hashgrid enlarge
    VT radius(n_spheres, 0.5);
    //    VT radius(blaze::linspace<blaze::rowVector>(n_spheres, 0.2, 0.5));

    //  VT mass(n_spheres, 1.0);
    VT mass = Material::getDensity(iron) * 4.0 / 3.0 * M_PI * radius * radius *
              radius;

    double final_time(24.0);
    std::size_t n_time_steps(20000UL);
    auto csv_writer =
        elastica::io::csv::make_writer("output", 50UL, 0, n_time_steps);

    {
      MT new_positions(positions + 1.0);
      VT new_mass(mass);
      MT new_velocities(velocities);
      VT new_radius(radius);
      sim->emplace_back<elastica::rigidbody::Sphere>(
          new_mass, new_positions, new_velocities, new_radius);
    }

    {
      MT new_positions = positions + 2.0;
      VT new_mass = mass + 0.0;
      MT new_velocities = velocities + 0.0;
      VT new_radius = radius + 0.0;
      sim->emplace_back<elastica::rigidbody::Sphere>(
          new_mass, new_positions, new_velocities, new_radius);
    }

    {
      auto plane_point = Vec3{0., 0., 0.};
      auto plane_normal = Vec3{0.0, 0., 1.0};
      using PP = elastica::collision::PlanePrimitive;
      using Spans = typename PP::Spans;
      Spans spans{100.0, 100.0};
      auto* plane = new elastica::collision::PlanePrimitive(
          0, 0, true, plane_point, plane_normal, spans, iron);
      sim->push_back_imposed(std::move(*plane));
      delete plane;
    }

    sim->add_simple_environment<environments::GravityField>();

    sim->finalize();

    integrate(
        timestepper::SystemCollectionStepper<timestepper::PositionVerlet>(),
        *sim, final_time, n_time_steps);
  } else if (case_number == 1) {
    // Do a small sphere atop a larger sphere atop a plane in one collection
    std::size_t n_spheres(2UL);
    constexpr std::size_t SMALL_IDX = 0UL;
    constexpr std::size_t BIG_IDX = 1UL;
    MT positions(3UL, n_spheres, 0.0);
    VT radius(n_spheres, 0.0);
    MT velocities(3UL, n_spheres, 0.0);

    // Big sphere characteristics
    radius[BIG_IDX] = 0.4;
    positions(2UL, BIG_IDX) = 0.1 + radius[BIG_IDX];

    // Small sphere characteristics
    const double offset(0.1);
    radius[SMALL_IDX] = 0.1;
    positions(2UL, SMALL_IDX) =
        positions(2UL, BIG_IDX) + radius[BIG_IDX] + offset + radius[SMALL_IDX];

    // Mass array
    VT mass = Material::getDensity(iron) * 4.0 / 3.0 * M_PI * radius * radius *
              radius;
    // Make spheres
    sim->emplace_back<elastica::rigidbody::Sphere>(mass, positions, velocities,
                                                   radius);

    // Plane charaacteristics
    auto plane_point = Vec3{0., 0., 0.};
    auto plane_normal = Vec3{0.0, 0., 1.0};
    using PP = elastica::collision::PlanePrimitive;
    using Spans = typename PP::Spans;
    Spans spans{100.0, 100.0};
    sim->emplace_back_imposed<PP>(0, 0, true, plane_point, plane_normal, spans,
                                  iron);

    // Add gravity field
    sim->add_simple_environment<environments::GravityField>();

    sim->finalize();

    double final_time(24.0);
    std::size_t n_time_steps(20000UL);
    auto csv_writer =
        elastica::io::csv::make_writer("output", 50UL, 0, n_time_steps);

    integrate(
        timestepper::SystemCollectionStepper<timestepper::PositionVerlet>(),
        *sim, final_time, n_time_steps);
  }

  /*
  std::size_t n_spheres(5UL);
  MT positions(3UL, n_spheres, 0.0);
  blaze::row(positions, 0UL) = blaze::linspace<blaze::rowVector>(n_spheres, 0.0,
  0.4); blaze::row(positions, 1UL) =
  blaze::linspace<blaze::rowVector>(n_spheres, 0.0, 0.5); blaze::row(positions,
  2UL) = blaze::linspace<blaze::rowVector>(n_spheres, 0.0, 0.8);
   */

  //  auto sphere_collection =
  //      std::make_shared<elastica::rigidbody::Sphere>(mass, positions,
  //      velocities, radius);
  //  sim->push_back<Sphere>(sphere_collection);

  //  std::size_t n_collections(5UL);
  //  for (std::size_t i_c = 0UL; i_c < n_collections; ++i_c) {
  //    MT new_positions(positions + i_c * 0.2);
  //    sim->push_back(std::move(*new elastica::rigidbody::Sphere(
  //        mass, new_positions, velocities, radius)));
  //  }
  //

  /*{
    MT new_positions(positions + 1.0);
    VT new_mass(mass);
    MT new_velocities(velocities);
    VT new_radius(radius);
    auto* sphere_collection = new elastica::rigidbody::Sphere(
        new_mass, new_positions, new_velocities, new_radius);
    sim->push_back(std::move(*sphere_collection));
    delete sphere_collection;
  }*/

  /*{
    MT new_positions(positions);
    auto* sphere_collection = new elastica::rigidbody::Sphere(
        mass, new_positions, velocities, radius);
    sim->push_back(std::move(*sphere_collection));
    delete sphere_collection;
  }*/

  //  {
  //    auto periodic_field = std::make_shared<environments::ExternalField>();
  //    sim->add_simple_environment(environment(periodic_field));
  //  }
  {
    //    auto* gravity_field = new environments::GravityField();
    //    sim->add_simple_environment(std::move(*gravity_field));
    //    delete gravity_field;
  }

  //  auto n_system_collections(sim->get_number_of_system_collections());
  //  auto sphere_vector = std::get<Sphere>(*sim);

  //  auto vtk_writer = elastica::io::make_writer("output", 1000UL, 10, 20,
  //  false); auto aabb_ = std::make_unique<AABB>(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
  //  vtk_writer->add(aabb_.get());
}

/*
template <typename T>
struct add_vector{
  using type = std::vector<T>;
};

int main(int argc, char* argv[]) {
  using my_list = tmpl::list<int, bool, char>;
  using my_transformed_list = tmpl::transform<my_list,
std::add_pointer<tmpl::_1>>;

  using my_further_transformed_list = tmpl::transform<my_transformed_list,
add_vector<tmpl::_1>>; using find_result =
tmpl::find<my_further_transformed_list, std::is_same<tmpl::_1, int**> >;
  // will print the types at runtime
//  tmpl::for_each<my_further_transformed_list>([](auto x)
//  {
//      std::cout << typeid(decltype(x)).name() << std::endl;
//  });

  using int_ptr_to_ptr_found = tmpl::found<my_further_transformed_list,
std::is_same<tmpl::_1, int**>>; static_assert(!int_ptr_to_ptr_found::value, "A
bool was found and I don't want bools!");
}

*/
