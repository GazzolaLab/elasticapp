//
// Created by Tejaswin Parthasarathy on 3/4/22.
//

#include <sstream>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/CollisionSystem/Contact/ContactType.hpp"
#include "Utilities/Invoke.hpp"
#include "Utilities/TMPL.hpp"

namespace {

  using namespace elastica;
  using namespace elastica::collision;

  ELASTICA_TEST_CASE("Unit.Environments.CollisionSystem.ContactType",
                     "[Unit][Environments]") {
    static constexpr std::size_t n_contact_types = 3UL;
    std::array<std::string, n_contact_types> all_typestrs{
        "colliding", "resting", "separating"};

    auto check_stream_for = [&](auto Is) {
      ContactType c{static_cast<ContactType>(Is.value)};
      std::stringstream ss;
      ss << c;
      CHECK(ss.str() == all_typestrs[Is]);
    };

    index_apply<n_contact_types>(
        [&](auto... Is) { EXPAND_PACK_LEFT_TO_RIGHT(check_stream_for(Is)); });
  }

}  // namespace
