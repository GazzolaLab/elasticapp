#pragma once

#include <iostream>

#include "Simulator/Configuration/ModuleConfiguration/BlockingPolicy.hpp"
#include "Utilities/PrettyType.hpp"

namespace elastica {

  namespace configuration {

    //**************************************************************************
    /*!\name stream execution policies*/
    /*!\ingroup blocking*/
    //@{
    template <typename BlockingPolicyRegistrars>
    std::ostream& operator<<(
        std::ostream& os,
        detail::BlockingPolicyBase<BlockingPolicyRegistrars> const& p) {
      using type = detail::BlockingPolicyBase<BlockingPolicyRegistrars>;
      call_with_dynamic_type<void, typename type::creatable_classes>(
          &p, [&os](auto const* const blocking_policy) {
            os << (*blocking_policy);
          });
      return os;
    }

    template <typename Criterion_, typename Registrars>
    std::ostream& operator<<(std::ostream& os,
                             BlockingPolicy<Criterion_, Registrars> const& p) {
      os << p.criterion_;
      return os;
    }

    template <typename Block>
    std::ostream& operator<<(std::ostream& os, RestrictSize<Block> const& p) {
      os << p.name() << "-" << pretty_type::name<Block>() << ":" << p.value();
      return os;
    }
    template <typename Block>
    std::ostream& operator<<(std::ostream& os,
                             LooselyRestrictSize<Block> const& p) {
      os << p.name() << "-" << pretty_type::name<Block>() << ":" << p.value();
      return os;
    }
    template <typename Block>
    std::ostream& operator<<(std::ostream& os, AlwaysNew<Block> const& p) {
      os << p.name() << "-" << pretty_type::name<Block>() << ":" << p.value();
      return os;
    }
    template <typename Block>
    std::ostream& operator<<(std::ostream& os, AlwaysSame<Block> const& p) {
      os << p.name() << "-" << pretty_type::name<Block>() << ":" << p.value();
      return os;
    }
    template <template <typename> class SizeBasedRestriction>
    std::ostream& operator<<(std::ostream& os,
                             AcrossBlockTypes<SizeBasedRestriction> const& p) {
      os << p.name() << ":" << p.value();
      return os;
    }
    //@}
    //**************************************************************************

  }  // namespace configuration

}  // namespace elastica
