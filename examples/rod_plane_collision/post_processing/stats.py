from dataclasses import dataclass, field
from typing import NewType, List

import numpy as np

PositiveInt = NewType("PositiveInt", int)


@dataclass
class Stats:
    contact_stiffness: float
    gravitational_acc: float
    mass: np.ndarray
    time: List[float] = field(default_factory=list)
    step: List[PositiveInt] = field(default_factory=list)
    position: List[np.ndarray] = field(default_factory=list)
    velocity: List[np.ndarray] = field(default_factory=list)
    translational_energy: List[float] = field(default_factory=list)
    rotational_energy: List[float] = field(default_factory=list)
    shearing_energy: List[float] = field(default_factory=list)
    bending_energy: List[float] = field(default_factory=list)
    penetration_depths: List[List[float]] = field(default_factory=list)
