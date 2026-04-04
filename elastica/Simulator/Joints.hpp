//==============================================================================
/*!
//  \file
//  \brief Joint interfaces for coupling systems in simulations
//
//  \ingroup joints
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once
#include <cstddef>

namespace YAML {
  template <typename T>
  struct convert;
}

namespace elastica {

  namespace joints {

    /*!
     * \brief Abstract base class for joints that apply coupled forces and
     * torques
     *
     * Concrete joints implement force and torque application rules over time.
     */
    class JointBase {
     protected:
      JointBase() = default;

      // Must come from the confgiuration files, leave for now
      using ValueType = double;
      using TimeType = ValueType;

    public:
      virtual ~JointBase() = default;
      //! Apply translational joint effects.
      virtual void apply_forces(ValueType& s, TimeType t) const = 0;
      //! Apply rotational joint effects.
      virtual void apply_torques(ValueType& s, TimeType t) const = 0;
    };

    //! Joint with spring and damping coefficients and fixed behavior hooks.
    struct FixedJoint : public JointBase {
      FixedJoint() : JointBase(){};
      FixedJoint(ValueType spring_constant, ValueType damping_coefficient)
          : JointBase(),
            spring_constant_(spring_constant),
            damping_coefficient_(damping_coefficient){};
      ~FixedJoint(){};
      virtual void apply_forces(ValueType& /*st*/,
                                TimeType /**/) const override{};
      virtual void apply_torques(ValueType& /*st*/,
                                 TimeType /**/) const override{};
      ValueType spring_constant_, damping_coefficient_;
    };

    //! Hinge-like joint variant derived from FixedJoint.
    class HingeJoint final : public FixedJoint {
      // Only access is by YAML-CPP, how do we protect against it?
      // friend struct YAML::convert<HingeJoint>;

     protected:
      HingeJoint(){};

     public:
      HingeJoint(ValueType spring_constant, ValueType damping_coefficient)
          : FixedJoint(spring_constant, damping_coefficient){};
      ~HingeJoint(){};
      virtual void apply_forces(ValueType& /*st*/,
                                TimeType /**/) const override{};
      virtual void apply_torques(ValueType& /*st*/,
                                 TimeType /**/) const override{};
    };

    //! Placeholder joint variant derived from FixedJoint.
    class FakeJoint final : public FixedJoint {
      // Only access is by YAML-CPP, how do we protect against it?
      // friend struct YAML::convert<FakeJoint>;

     protected:
      FakeJoint(){};

     public:
      FakeJoint(ValueType spring_constant, ValueType damping_coefficient)
          : FixedJoint(spring_constant, damping_coefficient){};
      ~FakeJoint(){};
      virtual void apply_forces(ValueType& /*st*/,
                                TimeType /**/) const override{};
      virtual void apply_torques(ValueType& /*st*/,
                                 TimeType /**/) const override{};
    };

  }  // namespace joints

}  // namespace elastica
