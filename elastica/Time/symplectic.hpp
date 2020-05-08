#pragma once

#include "SymplecticStepper/SymplecticStepperKernel.hpp"
#include "SymplecticStepper/SymplecticSystemCollectionStepper.hpp"
#include "SymplecticStepper/SymplecticSystemStepper.hpp"

#include "SymplecticStepper/Algorithms/PositionVerlet.hpp"

// Put anything in the "frontend"
namespace timestepper {

using symplectic::PositionVerlet;

template <class SymplecticAlgorithm>
using SystemStepper = symplectic::SystemStepper<
    symplectic::SymplecticStepperKernel<SymplecticAlgorithm>>;

template <class SymplecticAlgorithm>
using SystemCollectionStepper = symplectic::SystemCollectionStepper<
    symplectic::SymplecticStepperKernel<SymplecticAlgorithm>>;

} // namespace timestepper
