//
// Created by Tejaswin Parthasarathy on 6/2/21.
//

#include <string>
#include <type_traits>
#include <utility>

#include "Framework/TestingFramework.hpp"
#include "Utilities/AsConst.hpp"
#include "Utilities/TypeTraits/Cpp17.hpp"

namespace {

  ELASTICA_TEST_CASE("Unit.Utilities.as_const", "[Unit][Utilities]") {
    // [as_const_eg]
    std::string mutableString = "Hello World!";
    auto&& constRef = cpp17::as_const(mutableString);

    // OK
    // mutableString.clear();

    // error: 'constRef' is 'const' qualified,
    // but 'clear' is not marked const
    // constRef.clear();

    CHECK(&constRef == &mutableString);
    CHECK(&cpp17::as_const(mutableString) == &mutableString);

    using ExprType =
        std::remove_reference_t<decltype(cpp17::as_const(mutableString))>;

    static_assert(cpp17::is_same_v<std::remove_const_t<ExprType>, std::string>,
                  "ExprType should be some kind of string.");
    static_assert(!cpp17::is_same_v<ExprType, std::string>,
                  "ExprType shouldn't be a mutable string.");
    // [as_const_eg]
  }

}  // namespace