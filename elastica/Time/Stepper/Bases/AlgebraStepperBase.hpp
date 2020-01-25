#pragma once

template <class Algebra, class Operations>
class AlgebraStepperBase {
 public:
  using algebra_type = Algebra;
  using operations_type = Operations;

  AlgebraStepperBase(const algebra_type& algebra = algebra_type())
      : m_algebra(algebra) {}

  algebra_type& algebra() { return m_algebra; }

  const algebra_type& algebra() const { return m_algebra; }

 protected:
  algebra_type m_algebra;
};