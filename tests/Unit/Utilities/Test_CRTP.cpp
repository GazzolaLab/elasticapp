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
    void print() {
      std::cout << self().get_details() << "\t" << self().get_details() << "\t";
    }
  };

  struct IntegerPrinter : PrintInterface<IntegerPrinter> {
    int get_details() { return 2; }
  };

  struct FloatPrinter : PrintInterface<FloatPrinter> {
    float get_details() { return 2.1415F; }
  };
  /// [crtp_example]

  // [[OutputRegex, 2    2    2.1415    2.1415]]
  ELASTICA_TEST_CASE("Unit.Utilities.CRTP", "[Unit][Utilities]") {
    OUTPUT_TEST();
    IntegerPrinter().print();
    FloatPrinter().print();
  }

}  // namespace
