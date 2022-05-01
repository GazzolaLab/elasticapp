//
// Created by Tejaswin Parthasarathy on 4/20/22.
//
#include <algorithm>
#include <vector>

#include "Framework/TestingFramework.hpp"
#include "Simulator/Environments/Transfer/Transfer.hpp"

/// [customization_eg]
struct MySystem {
  int radius[6UL];
  int get_radius(std::size_t idx) const& { return radius[idx]; }
  int& get_radius(std::size_t idx) & { return radius[idx]; }
};
struct MyPrimitive {
  int radius;
  void transfer(int input) noexcept { radius = input; };
};

namespace elastica {

  namespace customization {

    template <>
    struct PrimitiveTrait<MySystem> {
      using type = MyPrimitive;
    };

    template <>
    struct Transfer<MySystem>
        : public ::elastica::collision::TransferTraits<MySystem> {
      // SystemType is an alias for MySystem
      // PrimitiveType is an alias for MyPrimitive
      // feel free to use them

      template <typename PrimitiveIterator>
      static void do_transfer(from_t /* meta */, SystemType const& collection,
                              to_t /* meta */, PrimitiveIterator begin,
                              PrimitiveIterator end) {
        // Uses primitives transfer member
        std::for_each(
            begin, end,
            [&collection, index = 0UL](PrimitiveType& primitive) mutable {
              primitive.transfer(collection.get_radius(index));
              ++index;
            });
      };

      template <typename ConstPrimitiveIterator>
      static void do_transfer(to_t /* meta */, SystemType& collection,
                              from_t /* meta */, ConstPrimitiveIterator begin,
                              ConstPrimitiveIterator end) {
        std::for_each(
            begin, end,
            [&collection, index = 0UL](PrimitiveType const& primitive) mutable {
              collection.get_radius(index) = primitive.radius;
              ++index;
            });
      }
    };

  }  // namespace customization

}  // namespace elastica

/// [customization_eg]

namespace {

  ELASTICA_TEST_CASE("Unit.Environments.Transfer",
                     "[Unit][Simulator][Environments]") {
    /// [transfer_eg]
    MySystem my_system{1, 2, 3, 4, 5, 6};
    std::vector<MyPrimitive> primitives(6UL, {0});

    using std::begin;
    using std::cbegin;
    using std::cend;
    using std::end;

    std::for_each(begin(primitives), end(primitives),
                  [&, i = 0UL](MyPrimitive const& p) mutable {
                    CHECK_FALSE(p.radius == my_system.get_radius(i++));
                  });

    elastica::collision::Transfer<MySystem>::do_transfer(
        elastica::collision::from, my_system, elastica::collision::to,
        begin(primitives), end(primitives));

    std::for_each(begin(primitives), end(primitives),
                  [&, i = 0UL](MyPrimitive const& p) mutable {
                    CHECK(p.radius == my_system.get_radius(i++));
                  });
    /// [transfer_eg]

    std::for_each(begin(primitives), end(primitives),
                  [&, i = 1UL](MyPrimitive& p) mutable {
                    p.radius = 4 * i;
                    ++i;
                  });

    std::for_each(begin(primitives), end(primitives),
                  [&, i = 0UL](MyPrimitive const& p) mutable {
                    CHECK_FALSE(p.radius == my_system.get_radius(i++));
                  });

    elastica::collision::Transfer<MySystem>::do_transfer(
        elastica::collision::to, my_system, elastica::collision::from,
        cbegin(primitives), cend(primitives));

    std::for_each(begin(primitives), end(primitives),
                  [&, i = 0UL](MyPrimitive const& p) mutable {
                    CHECK(p.radius == my_system.get_radius(i++));
                  });
  }

}  // namespace