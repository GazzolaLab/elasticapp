//
// Created by Tejaswin Parthasarathy on 12/30/20.
//

#include <iostream>

#include "Framework/TestingFramework.hpp"
#include "Utilities/CRTP.hpp"

namespace {
  /// [crtp_example]
  template <typename Detail>
  class PrintInterface : public elastica::CRTPHelper<Detail, PrintInterface> {
   private:
    using CRTP = elastica::CRTPHelper<Detail, PrintInterface>;
    using CRTP::self;

   public:
    // Constrain the interface here
    std::ostream& print(std::ostream& os) const {
      return os << self().get_details() << "    " << self().get_details()
                << "    ";
    }
  };
  template <typename T>
  std::ostream& operator<<(std::ostream& os,
                           PrintInterface<T> const& interface) {
    return interface.print(os);
  }

  struct CharAPrinter : PrintInterface<CharAPrinter> {
    char get_details() const { return 'a'; }
  };

  struct CharBPrinter : PrintInterface<CharBPrinter> {
    char get_details() const { return 'b'; }
  };
  /// [crtp_example]

  ELASTICA_TEST_CASE("Unit.Utilities.CRTP", "[Unit][Utilities]") {
    std::stringstream ss;
    ss << CharAPrinter() << CharBPrinter();
    CHECK(ss.str() == "a    a    b    b    ");
  }

}  // namespace
