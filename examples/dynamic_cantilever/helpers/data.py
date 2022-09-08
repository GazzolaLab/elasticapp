import numpy as np
from dataclasses import dataclass, field
from typing import NewType, List

from elasticapp.CaseStudies.PyDynamicCantileverCaseStudy import (
    DynamicCantileverCaseStudyParameters,
)

PositiveInt = NewType("PositiveInt", int)


@dataclass
class Stats:
    time: List[float] = field(default_factory=list)
    step: List[PositiveInt] = field(default_factory=list)
    position: List[np.ndarray] = field(default_factory=list)
    tip_deflection: List[float] = field(default_factory=list)


class CaseStudyParameters(DynamicCantileverCaseStudyParameters):
    def __init__(self, *args, **kwargs):
        self.mode = kwargs.pop("mode")
        super(CaseStudyParameters, self).__init__(*args, **kwargs)

    def __getstate__(self):
        return (DynamicCantileverCaseStudyParameters.__getstate__(self), self.__dict__)

    def __setstate__(self, state):
        p_state, t_state = state
        self.__dict__ = t_state
        DynamicCantileverCaseStudyParameters.__setstate__(self, p_state)


def setup_dynamic_cantilever_parameters(
    *,
    density: float = 2000.0,
    n_elements: PositiveInt = 100,
    mode: PositiveInt = 0,
) -> CaseStudyParameters:
    base_radius = 0.02
    base_length = 1.0
    youngs_modulus = 1e5
    shear_modulus = youngs_modulus / 3.0
    dt = 0.05 * (base_length / n_elements)

    return CaseStudyParameters(
        n_elements=n_elements,
        youngs_modulus=youngs_modulus,
        shear_modulus=shear_modulus,
        density=density,
        radius=base_radius,
        length=base_length,
        dt=dt,
        mode=mode,
    )


@dataclass
class RecordingParameters:
    sample_interval: int = 500
    fps: int = 30
