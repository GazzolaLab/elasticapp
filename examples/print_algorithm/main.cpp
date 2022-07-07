#include "Simulator/Connections.hpp"
#include "Simulator/Constraints.hpp"
#include "Simulator/Environments.hpp"
#include "Simulator/Forcing.hpp"
#include "Simulator/Simulator.hpp"
//
#include "Systems/CosseratRods.hpp"  // CosseratRod is a default system

template <typename... Tags>
std::ostream& display_tags(std::ostream& os,
                           tuples::TaggedTuple<Tags...> const& tagged_tuple) {
  auto display = [&os](auto tag) {
    os << pretty_type::short_name<decltype(tag)>() << ", ";
  };
  os << "(";
  EXPAND_PACK_LEFT_TO_RIGHT(display(Tags{}));
  os << ")";
  return os;
}

int main(int argc, char* argv[]) {
  /*
   *
   */
  namespace ec = elastica::configuration;

  auto simulator = elastica::make_simulator<
      elastica::modules::Connections, elastica::modules::Constraints,
      elastica::modules::Environments, elastica::modules::Forcing>();
  simulator->finalize();

  //  display_tags(std::cout, simulator->algorithms()) << std::endl;
  std::cout << simulator->algorithms() << std::endl;
  std::cout << simulator->algorithm_metadata() << std::endl;
}
