#pragma once
#include <utility>
#include <vector>

namespace elastica {

namespace boundary_conditions {
// Forward declare
/*
template <typename MLT, typename VLT>
class DirtyRodState;
template <typename MLT, typename VLT>
class DirtyRodDeriv;
 */

class BoundaryConditionBase {
protected:
  BoundaryConditionBase() = default;

  // Must come from the configuration files, leave for now
  using ValueType = double;
  using TimeType = ValueType;

public:
  virtual ~BoundaryConditionBase() = default;
  virtual void constrain_values(ValueType &s, TimeType t) const = 0;
  virtual void constrain_rates(ValueType &s, TimeType t) const = 0;
  virtual void set(std::vector<double> initial_positions,
                   std::vector<double> initial_velocities,
                   std::vector<double> initial_directors,
                   std::vector<double> initial_omegas) = 0;
};

struct FreeBoundaryCondition final : public BoundaryConditionBase {
  FreeBoundaryCondition(){};
  ~FreeBoundaryCondition(){};
  void constrain_values(ValueType & /*st*/, TimeType /**/) const
      noexcept override{};
  void constrain_rates(ValueType & /*dt*/, TimeType /**/) const
      noexcept override{};
  void set(std::vector<double> /*initial_positions*/,
           std::vector<double> /*initial_velocities*/,
           std::vector<double> /*initial_directors*/,
           std::vector<double> /*initial_omegas*/) noexcept override{};
};

struct FixedBoundaryCondition final : public BoundaryConditionBase {
  FixedBoundaryCondition(ValueType init_Q, ValueType init_x)
      : initial_Q(init_Q), initial_x(init_x){};
  // template <typename ...Args>
  // FixedBoundaryCondition(Args... args)
  // 	: initial_Q(std::get<0UL>(args...)), initial_x(init_x){};
  ~FixedBoundaryCondition(){};

  void constrain_values(ValueType &, TimeType /**/) const noexcept override{};
  void constrain_rates(ValueType &, TimeType /**/) const noexcept override{};

  void set(std::vector<double> /*initial_positions*/,
           std::vector<double> /*initial_velocities*/,
           std::vector<double> /*initial_directors*/,
           std::vector<double> /*initial_omegas*/) noexcept override{
      // initial_Q = initial_directors[0UL];
      // initial_x = initial_positions[0UL];
  };

  // Store starting Q and x?
  ValueType initial_Q, initial_x;
};

struct HingedBoundaryCondition final : public BoundaryConditionBase {
  // using Parent = DirtyBoundaryCondition<HingedBoundaryCondition>;
  explicit HingedBoundaryCondition(ValueType init_x) : initial_x(init_x){};
  ~HingedBoundaryCondition(){};

  //    template <typename StateType, typename TimeType>
  void constrain_values(ValueType & /**/, TimeType /**/) const
      noexcept override{};
  void constrain_rates(ValueType & /**/, TimeType /**/) const
      noexcept override{};

  void set(std::vector<double> /*initial_positions*/,
           std::vector<double> /*initial_velocities*/,
           std::vector<double> /*initial_directors*/,
           std::vector<double> /*initial_omegas*/) noexcept override{
      // initial_x = initial_positions[0UL];
  };

  // Store starting Q and x?
  ValueType initial_x;
};

} // namespace boundary_conditions

} // namespace elastica
