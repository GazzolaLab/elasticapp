//
// Created by Tejaswin Parthasarathy on 3/16/22.
//

#include "Simulator/Environments/CollisionSystem/Detection/Coarse/TypeTraits.hpp"
#include "Utilities/TMPL.hpp"

namespace {
  using elastica::collision::detection::coarse::tt::IsSamePrimitive;
  using elastica::collision::detection::coarse::tt::IsSamePrimitive_v;

  /// [dispatch_eg]
  struct Sphere;
  struct Box;

  static_assert(IsSamePrimitive<Sphere, Sphere>::value, "meta failure");
  static_assert(not IsSamePrimitive_v<Sphere, Box>, "meta failure");
  /// [dispatch_eg]

}  // namespace
