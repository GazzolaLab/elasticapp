//
// Created by Tejaswin Parthasarathy on 1/22/21.
//

#include "Simulator/Constraints/Constraint.hpp"
#include "Simulator/Constraints/Registration.hpp"
#include "Utilities/TMPL.hpp"

#include <type_traits>

namespace elastica {

  namespace constraints {

    ELASTICA_DEFINE_CONSTRAINT(ConstraintForTestingRegistration) {
     public:
      explicit ConstraintForTestingRegistration() noexcept = default;
      ConstraintForTestingRegistration(ConstraintForTestingRegistration &&
                                       other) noexcept = default;
      ~ConstraintForTestingRegistration() override = default;
      template <typename TimeType, typename System>
      void constrain_values(TimeType, System&, int) const {}
      template <typename TimeType, typename System>
      void constrain_rates(TimeType, System&, int) const {}
      static std::string name() noexcept {
        return "ConstrainForTestingRegistration";
      }
    };

  }  // namespace constraints

}  // namespace elastica

using ConstraintType =
    elastica::constraints::ConstraintForTestingRegistration<>;
static_assert(
    std::is_base_of<elastica::constraints::Constraint<
                        tmpl::list<elastica::constraints::Registrars::
                                       ConstraintForTestingRegistration>,
                        ConstraintType>,
                    ConstraintType>::value,
    "ELASTICA_DEFINE_CONSTRAINT() error!");