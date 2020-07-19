#pragma once

#include "elastica/Systems/RigidBodyBase.hpp"

namespace elastica {
//  namespace rigidbody {
//    class Sphere;
//  }

  namespace environments {

    struct EnvironmentBase {
     protected:
      EnvironmentBase() = default;

      // Must come from the confgiuration files, leave for now
      using ValueType = double;
      using TimeType = ValueType;

     public:
      virtual ~EnvironmentBase() = default;

      template <typename System>
      inline void apply(System& system, TimeType time_v);

      virtual void apply_sphere(::elastica::rigidbody::Sphere&,
                                TimeType time_v) const = 0;
    };

    template <>
    inline void EnvironmentBase::apply<::elastica::rigidbody::Sphere>(
        ::elastica::rigidbody::Sphere& sphere_collection, TimeType time_v) {
      apply_sphere(sphere_collection, time_v);
    }

    struct ExternalField : public EnvironmentBase {
      ExternalField() : EnvironmentBase(){};
      ~ExternalField() override = default;

      void apply_sphere(::elastica::rigidbody::Sphere& sphere_collection,
                        TimeType time_v) const override {
        auto& pos = sphere_collection.position_collection_;
        auto x = blaze::row(pos, 0UL);
        auto y = blaze::row(pos, 1UL);
        auto z = blaze::row(pos, 2UL);

//        auto prefactor(std::cos(2.0 * M_PI * time_v));
        auto prefactor(1.0);

        auto& force = sphere_collection.force_collection_;
        blaze::row(force, 0UL) = -4.0 * x;
        blaze::row(force, 1UL) = -25.0 * y;
//        blaze::row(force, 0UL) += 2.0 * blaze::pow(blaze::sin(M_PI * x), 2) *
//                                  blaze::sin(2.0 * M_PI * y) * blaze::sin(2.0 * M_PI * z);
//        blaze::row(force, 1UL) -= blaze::sin(2.0 * M_PI * x) *
//                                  blaze::pow(blaze::sin(M_PI * y), 2) *
//                                  blaze::sin(2.0 * M_PI * z);
//        blaze::row(force, 2UL) -= blaze::sin(2.0 * M_PI * x) *
//                                  blaze::pow(blaze::sin(M_PI * z), 2) *
//                                  blaze::sin(2.0 * M_PI * y);
        force *= prefactor;
//        blaze::row(force, 0UL) -= 2.0 * x;
      };
    };

  }  // namespace environments
}  // namespace elastica
